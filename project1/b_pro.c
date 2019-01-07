#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NT 1

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
//long g_number;
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
	{
		//new_node->num = num;
		//new_node->next = head->next;
		head->next = new_node;
		//printf("asdf");
	}
	else{
		Node * cur = head->next;
		while(cur->next != NULL)
			cur = cur->next;
		cur->next = new_node;

	//	printf("a%lda", head->next->next->next->num);
		//free(cur);
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
	//printf(" %ld를 디큐합니다.\n",num);
//	Node * del_node = (Node *)malloc(sizeof(Node)*1);
//	del_node = head->next;
	head->next = head->next->next;
	//if(head->next != NULL)
	//	free(head->next);
	
	return num;
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
	
	do{
	//pthread_cond_signal(&create_done);
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&create_done);
	//pthread_cond_signal(&create_done);
	//Idle_Count++;
	pthread_cond_wait(&master_done, &mutex);
	//if(done == true)
	//{	printf("break문 안\n");
	//	break;
	//}
	long number = Dequeue();
//	Idle_Count--;
	printf("[%x] update에서의 idle_count : %d, number : %ld \n",(unsigned int)tid, Idle_Count, number);
	pthread_mutex_unlock(&mutex);
	//sleep(number/10);
	sleep(number);
	pthread_mutex_lock(&mutex2);
	//printf("[%x] update에서의 idle_count : %d  g_number : %ld",(unsigned int)tid,Idle_Count,g_number);
	sum += number;
	if (number % 2 == 1)
		odd++;
	if (number < min)
		min = number;
	if (number > max)
		max = number;
	//printf("sum:%ld\n",sum);
	//sleep(number);
	pthread_mutex_unlock(&mutex2);
	
	pthread_mutex_lock(&mutex);
	Idle_Count++;
	printf("%ld 일끝남,  idle : %d\n", number, Idle_Count);
	pthread_cond_signal(&worker_done);
	pthread_mutex_unlock(&mutex);
	//pthread_cond_signal(&worker_done);
	printf("[%x] %ld\n",(unsigned int)tid,time(NULL)-a);
	}while(done == false);
	//printf("%d초, %x, 너빠져나왔니?\n",time(NULL)-a, (unsigned int)tid);

	return 0;
}

int main(int argc, char* argv[])
{
	Q_Init();
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
	Idle_Count = NT;
	//printf("idle_count : %d\n",Idle_Count);
	char *fn = argv[1];
	FILE *fin = fopen(fn, "r");
	char action;
	long num;
	a=time(NULL);
	while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
		if (action == 'p') {
			//g_number = num;
			//printf("Idle_Count : %d g_number : %ld\n",Idle_Count, g_number);
			//while(Idle_Count <= 0)
			//printf("idle: %d\n",Idle_Count);
			
			if(Idle_Count == 0)
			{
				printf("하이");
				pthread_mutex_lock(&mutex);
				pthread_cond_wait(&worker_done, &mutex);
				pthread_mutex_unlock(&mutex);
			}

			pthread_mutex_lock(&mutex);
			printf("%ld를 인큐합니다, %d는 Idle_Count\n",num,Idle_Count);
			Enqueue(num);
			//Idle_Count++;
			pthread_cond_signal(&master_done);
			Idle_Count--;
			pthread_mutex_unlock(&mutex);
			//Idle_Count--;
			//sleep(num);
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
	printf("최종 Idle Count : %d \n",Idle_Count);
	
	//pthread_cond_broadcast(&master_done);
	//for(int i=0; i<NT; i++)
	//{
	//	printf("여기오니?\n");
	//	pthread_join(p_thread[i], NULL);
	//}
	fclose(fin);
	
	printf("쓰레드가 종료되고, 파일이 닫혔습니다.\n");
	//stop = clock();
	//cputime = (double)(stop - start)/CLOCKS_PER_SEC;
	//printf("%ld %ld %ld %ld %f\n", sum, odd, min, max, cputime);
	sleep(10);
	printf("%ld %ld %ld %ld\n", sum, odd, min, max);
	
	return (EXIT_SUCCESS);
}
