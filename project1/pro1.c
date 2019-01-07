#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NT 8

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t idle_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t create_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t master_done = PTHREAD_COND_INITIALIZER;
pthread_cond_t worker_done = PTHREAD_COND_INITIALIZER;
pthread_cond_t create_done = PTHREAD_COND_INITIALIZER;

long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

int Idle_Count;
long g_number;
void *update();

void *update()
{
	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	printf("[%u] 프로세서에 [%x] thread 생성 완료\n", (unsigned int)pid, (unsigned int)tid);
	
	do{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&create_done);
	//Idle_Count++;
	pthread_cond_wait(&master_done, &mutex);
	//pthread_mutex_unlock(&mutex);

	//pthread_mutex_lock(&mutex);
	Idle_Count--;
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_lock(&mutex);
	long number = g_number;
	sleep(number/10);
	printf("[%x] update에서의 idle_count : %d  g_number : %ld",(unsigned int)tid,Idle_Count,g_number);
	
	//pthread_mutex_lock(&mutex);
	sum += number;
	if (number % 2 == 1)
		odd++;
	if (number < min)
		min = number;
	if (number > max)
		max = number;
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_lock(&idle_mutex);
	Idle_Count++;
	printf("idle : %d\n", Idle_Count);
	pthread_cond_signal(&worker_done);
	pthread_mutex_unlock(&idle_mutex);
	}while(done == false);
	printf("%x,너빠져나왔니?\n",(unsigned int)tid);
	
	return 0;
}

int main(int argc, char* argv[])
{
	//clock_t start, stop;
	//double cputime;
	//start = clock();
	pthread_t p_thread[NT];
	int thr_id = 0;
	
	if (argc != 2) {
		printf("Usage: sum <infile>\n");
		exit(EXIT_FAILURE);
	}
	
	for(int i = 0; i < NT; i++){
		pthread_mutex_lock(&create_mutex);
		thr_id = pthread_create(&p_thread[i], NULL, update, NULL);
		if(thr_id < 0){
			printf("Thread Creating Failure, Terminate...");
			exit(0);
		}
		pthread_cond_wait(&create_done, &create_mutex);
		pthread_mutex_unlock(&create_mutex);
	}
	Idle_Count=NT;
	printf("idle_count : %d\n",Idle_Count);
	char *fn = argv[1];
	FILE *fin = fopen(fn, "r");
	char action;
	long num;
	while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
		if (action == 'p') {
			//g_number = num;
			//printf("Idle_Count : %d g_number : %ld\n",Idle_Count, g_number);
			while(Idle_Count <= 0)
			{
				pthread_mutex_lock(&idle_mutex);
				pthread_cond_wait(&worker_done, &idle_mutex);
				pthread_mutex_unlock(&idle_mutex);
			}
			pthread_mutex_lock(&mutex);
			g_number = num;
			printf("%d %ld\n",Idle_Count, g_number);
			//Idle_Count++;
			pthread_cond_signal(&master_done);
			pthread_mutex_unlock(&mutex);
		} else if (action == 'w') {
			printf("나%ld초 만큼 기다린다.\n", num);
			sleep(num);
		} else {
			printf("ERROR: Unrecognized action: '%c'\n", action);
			exit(EXIT_FAILURE);
		}
	}
	if(Idle_Count != NT)
	{
		while(Idle_Count != NT);
	}
	done = true;
	printf("%d asdf\n",Idle_Count);
	pthread_cond_broadcast(&master_done);

	for(int i=0; i<NT; i++)
		pthread_join(p_thread[i], NULL);

	fclose(fin);
	
	printf("쓰레드가 종료되고, 파일이 닫혔습니다.\n");
	//stop = clock();
	//cputime = (double)(stop - start)/CLOCKS_PER_SEC;
	//printf("%ld %ld %ld %ld %f\n", sum, odd, min, max, cputime);
	printf("%ld %ld %ld %ld\n", sum, odd, min, max);

	return (EXIT_SUCCESS);
}
