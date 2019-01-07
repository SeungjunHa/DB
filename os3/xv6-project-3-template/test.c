#include "types.h"
#include "stat.h"
#include "user.h"
#include "memlayout.h"

void recursion(int n){
    if(n>0)
        recursion(n-1);
}
int func(int k){
    //      printf(1,"%x\n",&k);
    if(&k==(int*)(0x7FFFFFB0-0x20*(k-4))){
        printf(1,"Test%d\n",k+4);
    }else{
        printf(1,"Test%d Fail\n",k+4);
    }
    if(k==4) func(5);
    if(k==5) func(6);
    if(k>=6 && k<20) func(k+1);
    if(k==4 && &k!=(int*)0x7FFFFFB0) printf(1,"Test25 Fail\n");
    printf(1,"Test25\n");
    return k-1;
}
int func2(){
    int arr[6025]={0};
    if(arr[0]==0) arr[0]=0;
    printf(1,"Test 28 Fail\n");
    return 1;
}
int func3sub(int test){
    int k=0;
    //printf(1,"test %d : %x\n",test,&k);
    if(&k==(int*)0x7FDDD15C){
        printf(1,"Test%d\n",test);
    }else{
        printf(1,"Test%d Fail\n",test);
    }
    return 0;
}
int func3(){
    int i=0,pid;
    for(i=0;i<20;i++){
        pid=fork();
        if(pid==0){
            func3sub(i+29);
            exit();
        }
    }
    wait();
    return 0;
}
int* func4(){
    int *a =(int*)malloc(sizeof(int)*40);
    *a=2014312692;
    printf(1,"a : %x, I set the answer 0x9F60\n",a);
    if(a==(int*)0x9F60){
        printf(1,"Test 49 (it can be different)\n");
    }else{
        printf(1,"Test 49 Fail (it can be different)\n");
    }
    return a;
}
void growstack_by_recursion(int k){
    if(k>0) growstack_by_recursion(k-1);
    else func3();
}
    int
main(int argc, char **argv)
{
    int pid;
    int* p;
    int a=0;
    p=&pid;
    printf(1,"%x\n",p);
    if(p==(int*)0x7FFFFFC4){
        printf(1,"Test1\n");
    }else{
        printf(1,"Test1 Fail, %x\n",p);
    }
    pid=fork();
    if(pid==0){
        if(p==&pid){
            printf(1,"Test2\n");
        }else{
            printf(1,"Test2 Fail\n");
        }
        if(p==(int*)0x7FFFFFC4){
            printf(1,"Test3\n");
        }else{
            printf(1,"Test3 Fail\n");
        }
        if(&a==(int*)0x7FFFFFC0){
            printf(1,"Test4\n");
        }else{
            printf(1,"Test4 Fail\n");
        }
        int *b=(int*)0x80000000;
        *b=2;
        printf(1,"Test5 Fail\n");
    }
    wait();
    printf(1,"Test5\n");
    recursion(400);
    printf(1,"Test6\n");
    recursion(300);
    printf(1,"Test7\n");
    a=func(4);
    if(a==3)printf(1,"Test26\n");
    else printf(1,"Test26 Fail\n");
    printf(1,"%x == 0 ? is correct : is fail\n ",recursion);
    if(func==(void*)0x21) printf(1,"Test27\n");
    else printf(1,"Test27 Fail\n");
    pid=fork();
    if(pid==0){
        func2();
        exit();
    }
    wait();
    printf(1,"Test28\n");
    pid=fork();
    if(pid==0){
        growstack_by_recursion(70000);
        exit();
    }
    wait();
    pid=fork();
    if(pid==0){
        p=func4();
        if(*p==2014312692){
            printf(1,"Test50\n");
        }else{
            printf(1,"Test50 Fail\n");
        }
        exit();
    }
    wait();
    printf(1,"Finished all test. If there are some fail case, please let me know\n");
    exit();
}
