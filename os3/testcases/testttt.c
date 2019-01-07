#include "types.h"
#include "stat.h"
#include "user.h"
int g;
int func();
int main(int argc, char **argv){
    int* a=(int*)malloc(sizeof(int)*600);
    int* b=(int*)malloc(sizeof(int)*600);
    int* d=(int*)malloc(sizeof(int)*600);
    int* e=(int*)malloc(sizeof(int)*600);
    int* f=(int*)malloc(sizeof(int)*600);
    int* g=(int*)malloc(sizeof(int)*600);
    int* h=(int*)malloc(sizeof(int)*600);
    int* i=(int*)malloc(sizeof(int)*600);
    int* j=(int*)malloc(sizeof(int)*600);
    int* k=(int*)malloc(sizeof(int)*600);
    int* l=(int*)malloc(sizeof(int)*600);
    int* m=(int*)malloc(sizeof(int)*600);
    int* n=(int*)malloc(sizeof(int)*600);
    printf(1,"main : %x, data : %x   a-heap:%p b-heap:%p\n",main,&g,a,b);
    printf(1,"main : %x, data : %x   a-heal:%p b-heap:%p\n",main,&g,&a[599],&b[599]);
    printf(1,"%x %x %x %x %x %x %x %x %x %x %x %x %x",a,b,d,e,f,g,h,i,j,k,l,m,n);
    func();
    exit();
}
int func(){
    int k=0;
    int* c=(int*)malloc(sizeof(int)*600);
    printf(1,"func:%p c-heap:%p\n",&k,c);
    return 0;
}
