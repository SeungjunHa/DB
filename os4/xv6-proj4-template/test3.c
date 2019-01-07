#include "types.h"
#include "stat.h"
#include "user.h"

#define NTHREAD 8

void *stack[NTHREAD];
int tid[NTHREAD];
void *retval[NTHREAD];

void *thread(void *arg){
	thread_exit(arg);
}

void *thread2(void *arg){
	printf(1, "OK\n");
	exit();
}

int
main(int argc, char **argv)
{
	int i, j;

	printf(1, "TEST3: ");

	for(i=0;i<NTHREAD;i++)
		stack[i] = malloc(4096);

	for(i=0;i<100;i++){
		for(j=0;j<NTHREAD-1;j++){
			tid[j] = thread_create(thread, 30, (void *)j, stack[j]);
			if(tid[j] == -1){
				printf(1, "%d %d %d %d First WRONG\n", i, j, getpid(), gettid());
				exit();
			}
		}

		for(j=0;j<NTHREAD-1;j++){
			if(thread_join(tid[j], &retval[j]) == -1){
				printf(1, "Second WRONG\n");
				exit();
			}

			if((int)retval[j] != j){
				printf(1, "Third WRONG\n");
				exit();
			}
		}
	}

	for(i=0;i<NTHREAD-1;i++){
		tid[i] = thread_create(thread, 30, 0, stack[i]);
		if(tid[i] == -1){
			printf(1, "Fourth WRONG\n");
			exit();
		}
	}

	if(thread_create(thread, 30, 0, stack[NTHREAD-1]) != -1){
		printf(1, "Fifth WRONG\n");
		exit();
	}

	for(i=0;i<NTHREAD-1;i++){
		if(thread_join(tid[i], &retval[i]) == -1){
			printf(1, "Sixth WRONG\n");
			exit();
		}
	}

	for(i=0;i<NTHREAD-1;i++){
		tid[i] = thread_create(thread2, 30, 0, stack[i]);
		if(tid[i] == -1){
			printf(1, "Seventh WRONG\n");
			exit();
		}
	}

	thread_join(tid[0], &retval[0]);

	for(i=0;i<NTHREAD;i++)
		free(stack[i]);

	printf(1, "Eighth WRONG\n");

	exit();
}
