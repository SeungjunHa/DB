#include <stdio.h>
void copy(int, int*, int*);
int main(void)
{
    int arr[4]={ 1,2,3,4 };
    int test[4][4] = { 0,};
    int *a;
//    int arr2[1] = { &arr[0] };
    a = arr;
    copy(1,test[1],arr);
    printf("%d %d %d %d\n", a[0], a[1], a[2], a[3]);
    printf("%d %d %d %d\n", test[1][0], test[1][1], test[1][2], test[1][3]);
}
void copy(int index, int *copied, int *copy)
{
    for(int i=0;i<4;i++)
    {
        copied[i] = copy[i];
    }
}
