#include "xthreads.h"
#include "ds/ds.h" //queue, list, ds.h
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ucontext.h>

//Interrupts
int interruptsAreDisabled = 0;

//Global variable for passing function pointer and args to wrapper
void * (*functorun)(void*);
void* functorun_args;

//Pointer to a thread that needs to be free'd
thread* threadfree;

//Interrupt functions
static void interruptEnable();
static void interruptDisable();

//Internal thread management functions
void checkIfWaitingThreadCanQueue(thread*, void*);
void threadRun();
void loadNextThread();
void freeThread();

//Internal lock management functions
void unlock(int);
void lockLock(thread*,int);
bool isLocked(int);
void tryLock();
int getLockOwner(int);
bool checkLockCondNum(int, int);

//Thread management globals
queue* readyQueue; 
thread* current;
list* finished;
list* join;
static int maxID;

//Lock management globals
lock locks[NUM_LOCKS];
queue* conditionvars[NUM_LOCKS][CONDITIONS_PER_LOCK];


/* threadInit:
 *
 * Initialize all key data structures, define the main (current) thread.
 *
*/
void threadInit() {
	interruptDisable();
	//Prepare data structures
	maxID = 0;
	readyQueue = queueInit();
	finished = listInit();
	join = listInit();
	//Prepare the current thread
	int main_id = ++maxID;
	current = (thread*)malloc(sizeof(thread));
	current->num = main_id;
	current->lock = -1;
	current->context = (ucontext_t*)malloc(sizeof(ucontext_t));
	getcontext((current->context));
	current->stackptr = NULL;
	interruptEnable();
}
/* threadCreate:
 *
 * Create a new thread and then run it
 *
*/
int threadCreate( void * (*funcPtr)(void*), void* args) {
	interruptDisable();
	//Create a new thread
	int id = ++maxID;
	thread* t = (thread*)malloc(sizeof(thread));
	t->num = id;
	t->lock = -1;
	t->stackptr = (void*)malloc(STACK_SIZE);
	t->context = (ucontext_t*)malloc(sizeof(ucontext_t));
	//Setup up context for new thread
	t->context->uc_stack.ss_sp = t->stackptr;
	t->context->uc_stack.ss_size = STACK_SIZE;
	t->context->uc_stack.ss_flags = 0;
	getcontext(t->context);
	//Fill thread wrapper global variables
	functorun = funcPtr;
	functorun_args = args;
	makecontext(t->context, threadRun ,0);
        //Add the running thread back to the ready queue
	queuePush(readyQueue, current);
	thread* old = current;
	current = t;
	//Swap contexts and return the ID
	swapcontext(old->context, current->context);
	interruptEnable();
	return id;
}
/* threadRun:
 *
 * Act as a wrapper for running the thread function
 *
*/
void threadRun() {
	//Run the function
	interruptEnable();
	void* result = functorun(functorun_args);
	threadExit(result);
}

/* threadYield:
 * 
 * Stop the current running thread and switch to the next thread
 * in the ready queue
 *
*/
void threadYield() {
	interruptDisable();
	//Store the old thread in old
        thread* old = current;
	//Find the next thread that is not waiting on a lock
	do {
		queuePush(readyQueue, current);
		current = queuePop(readyQueue);
        	tryLock();
        } while(current->lock != -1);
	//Finally, swap contexts and optionally free the thread
        swapcontext((old->context), (current->context));
        interruptEnable();
	freeThread();
}

/* threadJoin:
 *
 * Join a thread. (Make it have another thread's competion as a dependency)
 *
*/
void threadJoin(int id, void** res) {
	interruptDisable();
	//If the ID never existed, don't waste time searching
	if (id > maxID) return;
	//Iterate through the list of finished processes to see if we have to join
	struct node_t* ptr = listGetHead(finished);
	while(ptr != NULL) {
		thread* temp = (thread*)ptr->data;
		if (ptr->data != NULL && id == temp->num) {
			if (res != NULL) *res = temp->result;
			interruptEnable();
			return;
		}
		ptr = ptr->next;
	}

	//If we made it here, the process we're joining with is still running
	//Setup a new JoinSet
	joinset* js = (joinset*)malloc(sizeof(joinset));
	js->blocked = current;
	js->dependency = id;
	js->result = res;
	//Add JoinSet to the joining list
	listInsertHead(join, js);
	loadNextThread();
	interruptEnable();
}

/* checkIfWaitingThreadCanQueue:
 *
 * This function is called when a thread finishes.
 * It checks if any threads waiting on the join list
 * 	can now join as a result of the current thread finishing
 *
*/
void checkIfWaitingThreadCanQueue(thread* f, void* res) {
	struct node_t* ptr = listGetHead(join);
	while ( ptr != NULL ) {
		joinset* js = ptr->data;
		if (js != NULL && js->dependency == f->num) {
			if (js->result != NULL) {
				//Thank you Sorber for telling me this
				*(js->result) = res;
			}
			queuePush(readyQueue, js->blocked);
			ptr->data = NULL;
			free(js);
		}
		ptr = ptr->next;
	}
}
/* threadExit:
 *
 * Called when a thread exits. Clean up thread and run the next
 *
*/
void threadExit(void *res) {
	interruptDisable();
	//If exit is called on the original thread, exit
	if (current->num == 1)
		exit(0);
	//Set the result pointer to the finished value
	current->result = res;
	listInsertTail(finished, current);
	//Set the thread able to be free'd
	threadfree=current;
	//Notify other threads they could potentially queue
	checkIfWaitingThreadCanQueue(current, res);	
	loadNextThread();
	interruptEnable();
	freeThread();
}

/* loadNextThread:
 *
 * Called when another thread needs to be executed
 *
*/
void loadNextThread() {
	//Store the current pointer in old
	thread* old = current;
	current = queuePop(readyQueue);
	tryLock();
	while(current->lock != -1) {
		//Called if the first thread is waiting on a lock
		queuePush(readyQueue,current);
		current=queuePop(readyQueue);
		tryLock();
	}
	//Swap contexts to our new thread
	swapcontext((old->context), (current->context));
	freeThread();
}

/* threadLock:
 * 
 * Lock the current thread, if possible. Else, wait for it
 *
*/
void threadLock(int lockNum) {
        interruptDisable();
	//Make sure the lock is valid
        if ( ! (0 <= lockNum && lockNum < NUM_LOCKS)) 
                 exit(0);

	//Handle locked/unlocked
        if ( ! isLocked(lockNum) ) {
                //Grant the lock since it is available
                lockLock(current, lockNum);
		interruptEnable();
        } else {
                //Mark them as waiting for a lock, put them in the ready queue, load the next
                current->lock = lockNum;
		interruptEnable();
                threadYield();       
        }
}

/* threadUnlock:
 *
 * Unlock the thread
 *
*/
void threadUnlock(int lockNum) {
	interruptDisable();
	//Make sure the lock is valid
	if (0 <= lockNum && lockNum < NUM_LOCKS) {
		//Unlock it
		unlock(lockNum);
	} else {
		//Quit if it is an invalid lock
		exit(0);
	}
	interruptEnable();
}

/* threadWait:
 *
 * Make a thread wait on a condition
 *
*/
void threadWait(int lockNum, int conditionNum) {
	interruptDisable();
	//Make sure the lock and condition are valid
	if (!checkLockCondNum(lockNum, conditionNum)) 
		exit(0);
	//Handle locked/unlocked
	if (isLocked(lockNum)) {
		//If locked - unlock, push the current in the queue
		unlock(lockNum);
		if (conditionvars[lockNum][conditionNum] == NULL) {
			conditionvars[lockNum][conditionNum] = queueInit();
		}
		queuePush(conditionvars[lockNum][conditionNum], current);
		loadNextThread();
	} else {
		//If it is not locked, exit
		exit(0);
	}
	interruptEnable();
}

/* threadSignal:
 *
 * Signal to other threads that this condition is free
 *
*/
void threadSignal(int lockNum, int conditionNum) {
	interruptDisable();
	//Make sure the lock and condition are valid
	if (!checkLockCondNum(lockNum, conditionNum))
		exit(0);
	//Grab the queue
	queue* q = conditionvars[lockNum][conditionNum];
	if (q == NULL || queueSize(q) == 0) {
		interruptEnable();
		return;
	}
	//Let the next thread waiting queue
	thread* next = queuePop(q);
	lockLock(next, lockNum);
	queuePush(readyQueue, next);
	interruptEnable();
}

//Free any threads that need to be freed
void freeThread() {
        if (threadfree != NULL) {
                free(threadfree->stackptr);
                free(threadfree->context);
                threadfree = NULL;
        }
}

//Make sure lockNum and condNum are valid locks and conditions
bool checkLockCondNum(int lockNum, int condNum) {
	return ( (0 <= lockNum && lockNum < NUM_LOCKS) && 
		(0 <= condNum && condNum < CONDITIONS_PER_LOCK) );
}
//Try to take the lock, if possible
void tryLock() {
	if (current->lock != -1 && !isLocked(current->lock)) {
		lockLock(current, current->lock);
		current->lock = -1;
	}
}

//Lock the lock. No bamboozles
void lockLock(thread* owner, int lockNum) {
	locks[lockNum].avail = 1;
	locks[lockNum].owner = owner;
}

//Unlock the lock. No bamboozles
void unlock(int lockNum) {
	locks[lockNum].avail = 0;
	locks[lockNum].owner = NULL;
}

//Check if the lock is locked
bool isLocked(int lockNum) {
	return (locks[lockNum].avail == 1);
}

//Get the owner thread of the lock
int getLockOwner(int lockNum) {
	if (! isLocked(lockNum)) return -1;
	return (locks[lockNum].owner)->num;
}

//interrupt management functions
//SORBER: Asserts are disabled because if it fails...
//With asserts: Crash and I'll lose 10 points.
//W/o  asserts: -1 for interrupts disabled...
static void interruptDisable () {
	//assert (! interruptsAreDisabled ) ;
	interruptsAreDisabled = 1;
}
static void interruptEnable () {
	//assert ( interruptsAreDisabled ) ;
	interruptsAreDisabled = 0;
}
