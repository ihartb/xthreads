#ifndef __ds_h__
#define __ds_h__

#include <ucontext.h>
#include "list.h"
#include "queue.h"

//Thread struct
typedef struct thread_t {
	ucontext_t* context;
	int num;
	int lock;
	void* stackptr;
	void* result;
} thread;

//Joinset struct
typedef struct join_t {
	thread* blocked;
	int	dependency;
	void** result;
} joinset;

//Lock struct
typedef struct lock_t {
	//avail == 0	FREE
	//avail == 1	LOCKED
	bool avail;
	thread* owner;
} lock;
#endif
