#include <stdio.h>
#include <stdlib.h>

#define N 2
double arr[N][N];
int reverse_arr[N][N];
int mul_arr[N-1][N][N];

int main(void)
{ 
    int i=0, j=0, k=0, cnt=0, p=0;
    for(i=0;i<N-1;i++) {
        for(j=0;j<N;j++) {
            mul_arr[i][j][j] = 1;
        }
    }

    arr[0][0] = 3;
    arr[0][1] = 6;
    //arr[0][2] = 2;
    arr[1][0] = 0;
    arr[1][1] = -17;
    //arr[1][2] = 3;
    //arr[2][2] = 6;
    /*
    arr[0][1] = 6.0;
    arr[0][2] = 4.0;
    arr[0][3] = 8.0;
    arr[1][2] = 5.0;
    arr[1][3] = 1.0;
    arr[2][3] = 2.0;
    */

    double **rev;
    rev = (double **)malloc(sizeof(double *)*N);
    for(i=0;i<N;i++)
            rev[i] = (double *)malloc(sizeof(double)*N);
    
    for(i=0;i<N;i++) {
//        for(j=0;j<N;j++) {
            rev[i][i] = 1/arr[i][i];
//        }
    }
    
    for(i=0;i<N;i++) {
        for(j=i+1;j<N;j++) {
            //if(i!=j)
                arr[i][j] = arr[i][j]/arr[i][i];
        }
        arr[i][i] = 1;
    }
   
    printf("---------------\n");
    double sum = 0;
    for(i=N-1;i>=0;i--) {
        for(j=N-1;j>i;j--) {
            for(k=N-1;k>i;k--) {
                sum -= rev[k][j] * arr[i][k]/arr[k][k];
            }
            rev[i][j] = sum;
            sum = 0;
        }
    }

    for(i=0;i<N;i++) {
        for(j=0;j<N;j++) {
            printf("%lf ",arr[i][j]);
        }
        printf("\n");
    }

    printf("-------------\n");

    for(i=0;i<N;i++) {
        for(j=0;j<N;j++) {
            printf("%lf ", rev[i][j]);
        }
        printf("\n");
    }
    return 0;
}
