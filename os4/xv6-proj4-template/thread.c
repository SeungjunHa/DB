#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

uint lock = 1;

void
lock_init(volatile uint *lock){
    *lock = 1;
}

void
lock_acquire(volatile uint *lock){
    while(1){
        if(xchg(lock, 0) == 0)
            break;
    }
}

void
lock_release(volatile uint *lock){
    xchg(lock, 1);
}

int thread_create(void *(*function)(void *), int priority, void *arg, void *stack){
	int tid;
    
    if((tid = clone(function, arg, (void *)stack)) != -1)
        return tid;
    return -1;    
}

void thread_exit(void *retval){
    why_exit(retval);    
}

int thread_join(int tid, void **retval){
    lock_init(&lock);
    lock_acquire(&lock);
    if(why_join(tid, retval) != tid)
        return -1;
    lock_release(&lock);
    return 1;
}

int gettid(void){
    return proc->tid;
}
