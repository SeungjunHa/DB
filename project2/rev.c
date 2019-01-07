#include <stdio.h>
#include <stdlib.h>

double *Lower_Inverse(double *);
int N;
int main()
{
    N = 14;
    double *arr = (double *)malloc(sizeof(double)*N*N);
    srand(4);
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            arr[i*N+j] = (double)(rand()%11);;
        }
    }
    for(int i=0;i<N;i++) {
        arr[i*N+i]=1.0d;
        for(int j=i+1;j<N;j++) {
            arr[i*N+j] = 0.0d;
        }
    }
    for(int i=0;i<N;i++) {
        for(int j=0;j<N;j++) {
            printf("%.1lf ",arr[i*N+j]);
        }
        printf("\n");
    }

    printf("Inverse : \n");
    arr = Lower_Inverse(arr);
    for(int i=0;i<N;i++) {
        for(int j=0;j<N;j++)
            printf("%.1lf ",arr[i*N+j]);
        printf("\n");
    }
    return 0;
}
double* Lower_Inverse(double *lower)
{
    int i,j,k;
    double *rev;
    rev = (double *)malloc(sizeof(double)*N*N);
    
    double sum = 0.0d;
    for(i=0;i<N;i++) {
        rev[i*N+i] = 1.0d;
        for(j=0;j<i;j++){
            for(k=0;k<i;k++)
                sum -= rev[k*N+j] * lower[i*N+k]/lower[k*N+k];
            rev[i*N+j] = sum;
            sum = 0.0d;
        }
    }
    return rev;
}
