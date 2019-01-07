#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#define num 10000 //1981까지 되고 1982부터는 Page Fault
int main(void)
{
    int stack[num];
    //int *heap, *heap2;
    //heap = (int *)malloc(sizeof(int)*num);
    //heap2 = (int *)malloc(sizeof(int)*num);
    int i=0;
    for(i=0;i<num;i++) {
    //    heap[i] = i;
    //    heap2[i] = i;
        stack[i] = 10+i;
    }
    printf(1, "[%p] \n", &i);
    //printf(1, "[%p %p %p]\n",main,stack,heap);
    for(i=0;i<num;i++){
        if(i == 0) 
            printf(1, "[1 : %d %p]\n", stack[i], &stack[i]);
        if(i==num-1)
            printf(1, "[9 : %d %p]\n", stack[i], &stack[i]);
    }
    stack[9999] = 1;
    //printf(1, "[%p %p %p]\n", stack[num], stack[num+1], stack[num+2]);
    /*for(i=0;i<num;i++){
        if(i==0){
            printf(1, "s[%d %p]\n", heap[i], &heap[i]);
            printf(1, "s[%d %p]\n", heap2[i], &heap2[i]);
        }
        if(i==num-1){ 
            printf(1, "e[%d %p]\n", heap[i], &heap[i]);
            printf(1, "e[%d %p]\n", heap2[i], &heap2[i]);   
        }
    }*/
    exit();
}
