#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
int N,SEED,T,FLAG;
typedef struct sparse {
    int value;
    int row;
    int column;
}SPA;

int main(int argc, char **argv)
{
    if(argc != 5) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
	N = atoi(argv[1]);
    SEED = atoi(argv[2]);
	T = atoi(argv[3]);
    FLAG = atoi(argv[4]);
    int i,j;
    omp_set_num_threads(T);

    int **A = (int **)malloc(sizeof(int *)*N);
    SPA **R = (SPA **)malloc(sizeof(SPA *)*(N-1));
    SPA *temp = (SPA *)calloc((N+1)*N/2, sizeof(SPA));
    for(i=0;i<N-1;i++) {
        R[i] = (SPA *)calloc(i+5, sizeof(SPA));
        for(j=0;j<N;j++) {
            R[i][j].value = 1;
            R[i][j].row = j;
            R[i][j].column = j;
        }
    }
    

    for(i=0;i<N;i++) {
        A[i] = (int *)calloc(N, sizeof(int));
    }
    
    srand(SEED);
    for(i=0;i<N;i++) {
        for(j=0;j<N;j++) {
            A[i][j] = rand()%10;
            if(j>i)
                A[i][j] = 0;
            else if(j==i)
                A[i][j] = 1;
            printf("   %d    ",A[i][j]);
        }
        printf("\n");
    }
    
    int **rev;
    rev = (int **)malloc(sizeof(int *)*(N-1));
    for(i=0;i<N-1;i++) {
        for(j=0;j<=i;j++) {
            rev[i] = (int *)malloc(sizeof(int)*(j+1));
        }
    }
    
    for(i=0;i<N-1;i++) {
        for(j=0;j<i+1;j++) {
            rev[i][j] = -A[N-1-j][N-2-i];
            R[i][N+j].value = rev[i][j];
            R[i][N+j].row = N-1-j;
            R[i][N+j].column = N-2-i;
        }
    }

    /*for(i=0;i<N-1;i++) {
        for(j=0;j<N+i+1;j++) {
            printf("(%d %d %d) ",R[i][j].value,R[i][j].row,R[i][j].column);
        }
        printf("\n");
    }*/
   
    for(i=0;i<(N+1)*N/2;i++) {
            temp[i].value = 1;
            temp[i].row = i;
            temp[i].column = i;
            printf("(%d %d %d)",temp[i].value,temp[i].row,temp[i].column);
    }

    for(i=0;i<N-1;i++) {
        for(j=0;j<N+i+1;j++) {
            if(R[i][N+j].column == temp[j].row){
                temp[j].value = R[i][N+j].value * temp[j].value;
                temp[j].row = R[i][N+j].row;
                temp[j].column = temp[j].column;
            }
        }
    }

    printf("--------------\n-------------");
    printf("\n");
    for(i=0;i<N-1;i++) {
        for(j=0;j<=i;j++) {
            printf("   %d   ",rev[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}
