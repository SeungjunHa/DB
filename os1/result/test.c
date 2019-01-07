#include "types.h"
#include "stat.h"
#include "user.h"
  
int test1()
{
	int pid = getpid();
	setnice(pid, 9);
	int nice = getnice(pid);
	nice = getnice(pid);
	if(nice == 9) {
		printf(1, "TEST1: CORRECT\n");
		return 1;
	}
	else{
		printf(1, "TEST1: wrong... invalid nice value\n");
		return 0;
	}
}
 
int test2()
{
	int pid = getpid();
	int nice = getnice(pid);
	if(nice != 20) {  
		printf(1, "TEST2: wrong... invalid default nice value\n");
		return 0;
	}

	setnice(pid, -10);  // invalid nice value check
	setnice(pid, 41);  // invalid nice value check
	nice = getnice(pid);
	if(nice != 20) {
		printf(1, "TEST2: wrong... invalid nice value range\n");
		return 0;
	}
	printf(1, "TEST2: CORRECT\n");
	return 1;
}
 
int test3()
{
	int ppid = getpid();
	setnice(ppid, 1);

	int pid = fork();
	if(pid==0) {  // child
		int cpid = getpid();
		setnice(cpid, 7);
		while(1);
		return 0;
	}
	else { // parent
		sleep(10);
		int pnice = getnice(ppid);
		int cnice = getnice(pid);
		if(pnice==1 && cnice == 7){
			printf(1, "TEST3: CORRECT\n");
			kill(pid);
			return 1;
		}
		else{
			printf(1, "TEST3: wrong... invalid parent and child nice values\n");
			kill(pid);
			return 0;
		}
	}
}
  
int test4()
{
	int pid_arr[10];

	for(int i=0;i<10;i++){
		int pid = fork();
		if(pid==0){
			while(1);
			return 0;
		}
		else{
			pid_arr[i] = pid;
		}
	}

	for(int i=0;i<10;i++){
		setnice(pid_arr[i], i);
	}

	for(int i=0;i<10;i++){
		if(i != getnice(pid_arr[i])){
			printf(1, "TEST4: wrong... invalid child nice values\n");
			for(int j=0;j<10;j++){
				kill(pid_arr[j]);
			}
			return 0;
		}
	}
	for(int j=0;j<10;j++){
		kill(pid_arr[j]);
	}
	printf(1, "TEST4: CORRECT\n");
	return 1;
}
 
int test5()
{
	int nproc=3;
	int pid_arr[nproc];

	for(int i=0;i<nproc;i++){
		int pid = fork();
		if(pid==0){
			while(1);
		} 
		else{
			pid_arr[i] = pid;
		}
	}

	for(int i=0;i<nproc;i++){
		setnice(pid_arr[i], i);
	}

	printf(1, "TEST5: \n");
	for(int i=0;i<nproc;i++){
		ps(pid_arr[i]);
	}
	//ps(0);
	for(int j=0;j<10;j++){
		kill(pid_arr[j]);
	}


	return 1;
}

int main(){
	int pid=getpid(),cpid;
	int retval;
	printf(1,"%d(20)\n",getnice(pid));
	retval= setnice(pid,8);
	printf(1,"%d(8),%d(0)\n",getnice(pid),retval);
	retval= setnice(pid,-1);
	printf(1,"%d(8),%d(-1)\n",getnice(pid),retval);
	retval= setnice(pid,41);
	printf(1,"%d(8),%d(-1)\n",getnice(pid),retval);
	retval= setnice(124,7);
	printf(1,"%d(8),%d(-1)\n",getnice(pid),retval);
	ps(pid);
	printf(1,"notthing:");ps(12444);
	printf(1,"\n");
	printf(1,"%d(-1)\n",getnice(117));
	cpid=fork();
	if(cpid==0){
		printf(1,"%d(20)\n",getnice(getpid()));
		retval=setnice(getpid(),2);
		printf(1,"%d(2),%d(0)",getnice(getpid()),retval);
		while(1);
		return 0;
	}else{
		sleep(10);
		printf(1,"%d(2),%d(8)",getnice(cpid),getnice(pid));
		ps(cpid);
		kill(cpid);
		ps(0);
	}
	int arr[5];
	for(int i=0;i<5;i++){
		int child=fork();
		if(child==0){
			while(1);
			return 0;
		}else{
			arr[i]=child;
		}
	}
	for(int i=0;i<5;i++){
		retval=setnice(arr[i],i*12);
	}
	for(int i=0;i<5;i++){
		printf(1,"%d(%d)",getnice(arr[i]),i*12>40?20:i*12);
		printf(1,"%d(-1)\n",retval);
		kill(arr[i]);
	}
	sleep(20);
	ps(0);
	exit();
}
