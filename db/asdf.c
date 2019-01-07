#include <stdio.h>

int main()
{
    int a = 5;
    while(a>=0) {
        a--;
    }
    printf("%d ",a);
    a=5;
    while(a>=0 && a--);
    printf("%d ",a);
}
