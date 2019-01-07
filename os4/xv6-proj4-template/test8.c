#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NTHREAD 7

void *stack[NTHREAD];
int tid[NTHREAD];
void *retval[NTHREAD];

void *thread1(void *arg){
	int *fd = (int *)arg;

	*fd = open("testfile", O_CREATE|O_RDWR);
	if(*fd < 0){
		printf(1, "WRONG\n");
		exit();
	}

	if(write(*fd, "hello", 5) != 5){
		printf(1, "WRONG\n");
		exit();
	}

	thread_exit(0);
}

void *thread2(void *arg){
	int fd = (int)arg;

	if(write(fd, "world", 5) != 5){
		printf(1, "WRONG\n");
		exit();
	}

	close(fd);

	thread_exit(0);
}

int
main(int argc, char **argv)
{
	int i;
	int fd;
	char buf[100];

	printf(1, "TEST8: ");

	for(i=0;i<NTHREAD;i++)
		stack[i] = malloc(4096);

	if((tid[0] = thread_create(thread1, 10, (void *)&fd, stack[0])) == -1){
		printf(1, "WRONG\n");
		exit();
	}
	if(thread_join(tid[0], &retval[0]) == -1){
		printf(1, "WRONG\n");
		exit();
	}
	if((tid[1] = thread_create(thread2, 10, (void *)fd, stack[1])) == -1){
		printf(1, "WRONG\n");
		exit();
	}
	if(thread_join(tid[1], &retval[1]) == -1){
		printf(1, "WRONG\n");
		exit();
	}

	fd = open("testfile", O_RDONLY);
	if(read(fd, buf, 10) != 10){
		printf(1, "WRONG\n");
		exit();
	}

	if(strcmp(buf, "helloworld") != 0){
		printf(1, "WRONG\n");
		exit();
	}

	for(i=0;i<NTHREAD;i++)
		free(stack[i]);

	printf(1, "OK\n");

	exit();
}
