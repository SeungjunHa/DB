#include "types.h"
#include "stat.h"
#include "user.h"

#define NTHREAD 7

void *stack[NTHREAD];
int tid[NTHREAD];
void *retval[NTHREAD];
int mem;

void *thread(void *arg){
	if(mem != (int)arg){
		printf(1, "[%d %d]WRONG\n", mem, (int)arg);
		exit();
	}

	thread_exit(0);
}

int
main(int argc, char **argv)
{
	int i;

	printf(1, "TEST7: ");

	for(i=0;i<NTHREAD;i++)
		stack[i] = malloc(4096);

	for(i=0;i<NTHREAD;i++){
		mem = i;
		tid[i] = thread_create(thread, 10, (void *)i, stack[i]);
		if(tid[i] == -1){
			printf(1, "1 WRONG\n");
			exit();
		}
	}

	for(i=0;i<NTHREAD;i++){
		if(thread_join(tid[i], &retval[i]) == -1){
			printf(1, "2 WRONG\n");
			exit();
		}
	}

	for(i=0;i<NTHREAD;i++)
		free(stack[i]);

	printf(1, "OK\n");

	exit();
}
