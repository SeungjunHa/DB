#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t create_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t master_done = PTHREAD_COND_INITIALIZER;
pthread_cond_t worker_done = PTHREAD_COND_INITIALIZER;
pthread_cond_t create_done = PTHREAD_COND_INITIALIZER;

long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

typedef struct _node{
	long num;
	struct _node * next;
}Node;

int Idle_Count;
void *update();
void Enqueue(long );
long Dequeue();
int Q_IsEmpty();
void Q_Init();
time_t a;
Node * head;

void Enqueue(long num)
{
	Node * new_node = (Node *)malloc(sizeof(Node)*1);
	new_node->num = num;
	new_node->next = NULL;
	
	if(Q_IsEmpty())
		head->next = new_node;
	else{
		Node * cur = head->next;
		while(cur->next != NULL)
			cur = cur->next;
		cur->next = new_node;
	}
	return;
}

long Dequeue()
{
	if(Q_IsEmpty()){
		printf("Queue is Empty!\n");
		return -1;
	}
	long num = head->next->num;
	head->next = head->next->next;
	
	return num;
}
int Q_num()
{
	int cnt = 0;
	Node * cur = head->next;
	while(cur != NULL)
	{
		cnt++;
		cur = cur->next;
	}
	return cnt;
}
int Q_IsEmpty()
{
	if(head->next == NULL)
		return 1;
	else
		return 0;
}

void Q_Init()
{
	head = (Node *)malloc(sizeof(Node)*1);
	head->next = NULL;
}

void *update()
{
	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	//printf("[%u] 프로세서에 [%x] thread 생성 완료\n", (unsigned int)pid, (unsigned int)tid);
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&create_done);
	pthread_cond_wait(&master_done,&mutex);
	do{
	
	//pthread_cond_signal(&create_done);
	//pthread_cond_wait(&master_done, &mutex);
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&mutex);
	if(done == true)
	{
	//	printf("%x, break문 안\n",(unsigned int)tid);
		pthread_mutex_unlock(&mutex);
		break;
	}
	long number = Dequeue();
	//printf("[%x] update에서의 idle_count : %d, number : %ld \n",(unsigned int)tid, Idle_Count, number);
	pthread_mutex_unlock(&mutex);
	
	sleep(number);

	pthread_mutex_lock(&mutex2);
	sum += number;
	if (number % 2 == 1)
		odd++;
	if (number < min)
		min = number;
	if (number > max)
		max = number;
	pthread_mutex_unlock(&mutex2);

	pthread_mutex_lock(&mutex);
	Idle_Count++;
//	printf("%ld 일끝남,  idle : %d ", number, Idle_Count);
	pthread_cond_signal(&worker_done);
	
//	printf("걸린 시간 : [%x] %ld\n",(unsigned int)tid,time(NULL)-a);
	}while(done == false);

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc != 3){
		printf("Usage: sum <infile> <Number of Threads>\n");
		exit(EXIT_FAILURE);
	}
	Q_Init();
	int NT = atoi(argv[2]);
	pthread_t p_thread[NT];
	int thr_id = 0;
	
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
	Idle_Count = NT;
	
	char *fn = argv[1];
	FILE *fin = fopen(fn, "r");
	char action;
	long num;
	a=time(NULL);
	
	while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
		if (action == 'p') {
			pthread_mutex_lock(&mutex);
			if(Idle_Count == 0)
				pthread_cond_wait(&worker_done, &mutex);

			//printf("%ld를 인큐합니다, %d는 Idle Count \n", num, Idle_Count);
			Enqueue(num);
			pthread_cond_signal(&master_done);
			Idle_Count--;
			pthread_mutex_unlock(&mutex);
		} else if (action == 'w') {
			sleep(num);
			//printf("나%ld초 만큼 기다린다.\n", num);
		} else {
			printf("ERROR: Unrecognized action: '%c'\n", action);
			exit(EXIT_FAILURE);
		}
	}

	//printf("aaa\n");
	while(Idle_Count < NT);


	pthread_mutex_lock(&mutex);
	done = true;
	pthread_mutex_unlock(&mutex);

	if(Idle_Count == NT)
		pthread_cond_broadcast(&master_done);
	
	for(int i=0; i<NT; i++)
		pthread_join(p_thread[i], NULL);
	
	fclose(fin);
	
	printf("%ld %ld %ld %ld\n", sum, odd, min, max);
	
	return (EXIT_SUCCESS);
}
