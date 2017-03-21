# xthreads
### User mode thread library written in C

### Installation
1. Clone repository on to local machine
2. run ~~~make~~~
3. Compile your library with xthreads.a

### API 
    threadInit()
Must call before you use any threads. This initializes all data structures for usage.

    threadCreate( void * (*)(void*), void*)
Create a new thread and switch contexts to it. Accepts any function (argument is void pointer, returns void pointer). Returns an integer that is the new thread's unique ID

    threadYield()
The current threading thread will stop running and allow the next thread in the running queue to execute.

    threadJoin(int, void**)
Join the current running thread with the thread ID specified in the argument. The result of the process blocking the current one will be stored in the second argument.

    threadExit()
Call this method when your thread is done running and can safely be terminated. Argument is the exit code of the current process (code given to threadJoin)

    threadLock(int)
Attempt to obtain a specific lock. The thread will halt until it receives the lock.

    threadUnlock(int)
Unlock a lock that the current thread holds.

    threadWait(int, int)
Wait on a specific condition on a specific lock. Arguments are the lock number and the condition number.

    threadSignal(int, int)
Signal all threads waiting on a specific condition on a specific lock. This will allow the next thread waiting to execute.

### Known Issues
1. This library is not deadlock-proof. You can deadlock threads if you are not careful.
2. Library does not yet support running on multiple cores in parallel. Threads are run concurrently.
3. It could be faster. Creating another queue for locks would help. All operations are still performed in O(n) time.

### Usage
1. If you found this repository, you are probably looking for code or ideas to steal. I advise against this as it is a violation of the honor code and Dr. Sorber does look very closely at design patterns. 
