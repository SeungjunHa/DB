#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define BLOCK_SIZE 32
#define SQU_BLO 1024

double *A;
int N_, N, NN, T;
int NUM_OF_BLOCK, TOTAL_NUM_OF_BLOCK;

void flow(double **, double **, double **, int, int, int, int);
void LU_DE(double *, double *, double *, int);
void mul(int , double *, double *, double *);
void mul_l(int, double *, double *, double *);
void mul_u(int, double *, double *, double *);
double *Upper_Inverse(double *);
double *Lower_Inverse(double *);
void print(double **, double **, int);
void copy(int, double *, double *);
int transpose(int);

int main(int argc, char *argv[])
{
	if(argc != 5) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
    N_ = atoi(argv[1]);
	T = atoi(argv[3]);    
   
    int padding;
    N = N_;
    padding = N % BLOCK_SIZE;
    if(padding)
        N = N_ + BLOCK_SIZE-padding;
    NUM_OF_BLOCK = N/BLOCK_SIZE;
    TOTAL_NUM_OF_BLOCK = NUM_OF_BLOCK*NUM_OF_BLOCK;
    NN = N*N;
    A = (double *)calloc(NN, sizeof(double));
    
    srand(atoi(argv[2]));
    for(int i=0; i<N_; i++)
		for(int j=0; j<N_; j++) {
			A[i*N+j] = (double)rand();
        }
    
    for(int i=N_;i<N;i++)
        A[i*N+i] = 1.0d;
    double **A_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    double **L_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    double **U_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    for(int i=0;i<TOTAL_NUM_OF_BLOCK;i++) {
        A_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        L_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        U_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        copy(i, A_BLOCK[i], A);
    }
    
    //double start = omp_get_wtime();    
    int BODY;
    int STAGE = 1;
    int WING = NUM_OF_BLOCK;
    int temp = NN/SQU_BLO;
    for(int i=0; i<temp; i+=NUM_OF_BLOCK+1) {
        for(int j=0; j<SQU_BLO; j+=BLOCK_SIZE+1)
            L_BLOCK[i][j] = 1.0d;
        WING--;
        BODY = WING*WING;
        flow(A_BLOCK, L_BLOCK, U_BLOCK, i, WING, BODY, STAGE);
        STAGE++;
    }
    //printf("Time : %lf\n", omp_get_wtime()-start);

    if(atoi(argv[4]))
        print(L_BLOCK, U_BLOCK, padding);
    return 0;
}

void flow(double **A_BLOCK, double **L_BLOCK, double **U_BLOCK, int index, int WING, int BODY, int STAGE)
{
    LU_DE(A_BLOCK[index], L_BLOCK[index], U_BLOCK[index], index);

    double * L_inv = (double*)calloc(SQU_BLO, sizeof(double));
    L_inv = Lower_Inverse(L_BLOCK[index]);

    double * U_inv = (double *)calloc(SQU_BLO, sizeof(double));
    U_inv = Upper_Inverse(U_BLOCK[index]);

#pragma omp parallel for schedule(auto) num_threads(T)
    for(int i=1; i<=WING; i++) {
        int simple_l = index+i*NUM_OF_BLOCK;
        mul_l(simple_l, A_BLOCK[simple_l], U_inv, L_BLOCK[simple_l]);
        int simple_u = index+i;
        mul_u(simple_u, L_inv, A_BLOCK[simple_u], U_BLOCK[simple_u]);
    }    
    
#pragma omp parallel for schedule(auto) num_threads(T)
    for(int i=1; i<=BODY; i++){
        int A_pos = index + NUM_OF_BLOCK + i + STAGE*((i-1)/WING);
        int temp = A_pos % NUM_OF_BLOCK;
        int L_pos = A_pos - temp + (STAGE-1);
        int U_pos = temp + (STAGE-1)*NUM_OF_BLOCK;
        mul(A_pos, A_BLOCK[A_pos], L_BLOCK[L_pos], U_BLOCK[U_pos]);
    }
}

void LU_DE(double *A_BLOCK, double *L_BLOCK, double *U_BLOCK, int index) 
{
    for(int k=0;k<BLOCK_SIZE;k++) {
		U_BLOCK[k*BLOCK_SIZE+k] = A_BLOCK[k*BLOCK_SIZE+k];
        for(int i=k+1;i<BLOCK_SIZE;i++) {
			L_BLOCK[i*BLOCK_SIZE+k] = A_BLOCK[i*BLOCK_SIZE+k]/U_BLOCK[k*BLOCK_SIZE+k];
			U_BLOCK[k*BLOCK_SIZE+i] = A_BLOCK[k*BLOCK_SIZE+i];
		}
		for(int i=k+1;i<BLOCK_SIZE;i++) {
			for(int j=k+1;j<BLOCK_SIZE;j++)
				A_BLOCK[i*BLOCK_SIZE+j]-=L_BLOCK[i*BLOCK_SIZE+k]*U_BLOCK[k*BLOCK_SIZE+j];
        }
    }
}

void mul(int pos, double *A_BLO, double *L_BLO, double *U_BLO)
{    
    double sum = 0.0d;
    for(int i=0; i<BLOCK_SIZE; i++) {
        for(int j=0; j<BLOCK_SIZE; j++){
            for(int k=0; k<BLOCK_SIZE; k++)
                sum += L_BLO[i*BLOCK_SIZE+k]*U_BLO[k*BLOCK_SIZE+j];
            A_BLO[i*BLOCK_SIZE+j] -= sum;
            sum = 0.0d;
        }
    }
}
                
void mul_u(int pos, double *L_inv, double *A_BLO, double *U_BLO)
{     
    double sum = 0.0d;
    for(int i=0;i<BLOCK_SIZE;i++) {
        for(int j=0;j<BLOCK_SIZE;j++) {
            for(int k=0;k<BLOCK_SIZE;k++)
                sum += L_inv[i*BLOCK_SIZE+k]*A_BLO[k*BLOCK_SIZE+j];
            U_BLO[i*BLOCK_SIZE+j] = sum;
            sum = 0.0d;
        }
    }
}

void mul_l(int pos, double *A_BLO, double *U_inv, double *L_BLO)
{
    double sum = 0.0d;
    for(int i=0;i<BLOCK_SIZE;i++) {
        for(int j=0;j<BLOCK_SIZE;j++) { 
            for(int k=0;k<BLOCK_SIZE;k++)
                sum += A_BLO[i*BLOCK_SIZE+k]*U_inv[k*BLOCK_SIZE+j];
            L_BLO[i*BLOCK_SIZE+j] = sum;
            sum = 0.0d;
        }
    }
}

double* Upper_Inverse(double *upper)
{
    double *rev = (double*)calloc(SQU_BLO, sizeof(double));
    double sum = 0.0d;

    for(int i=0; i<BLOCK_SIZE;i++){
        rev[i*BLOCK_SIZE+i] = 1.0d / upper[i*BLOCK_SIZE+i];
        for(int j=i-1;j>=0;j--){
            sum = 0.0d;
            for(int k=j+1;k<=i;k++)
                sum += upper[j*BLOCK_SIZE+k]*rev[k*BLOCK_SIZE+i];
            rev[j*BLOCK_SIZE+i] = -sum/upper[j*BLOCK_SIZE+j];
        }
    }
    return rev;
}

double* Lower_Inverse(double *lower)
{    
    double *rev = (double *)calloc(SQU_BLO, sizeof(double));
    double sum = 0.0d;
    for(int i=0;i<BLOCK_SIZE;i++)
        rev[i*BLOCK_SIZE+i] = 1.0d;

    for(int i=1;i<BLOCK_SIZE;i++) {
        for(int j=i;j<BLOCK_SIZE;j++) {
            for(int k=j-i;k<=j;k++){
                sum += rev[j*BLOCK_SIZE+k]*lower[k*BLOCK_SIZE+j-i];
            }
            rev[j*BLOCK_SIZE+j-i] = -sum/lower[(j-i)*BLOCK_SIZE+j-i];
            sum = 0.0d;
        }
    }
    return rev;
}

void print(double **L_BLO, double **U_BLO, int padding)
{      
    double *L = (double *)malloc(sizeof(double)*NN);
    double *U = (double *)malloc(sizeof(double)*NN);
    for(int i=0; i<TOTAL_NUM_OF_BLOCK;i++) {
        for(int k=0;k<BLOCK_SIZE;k++) {
            for(int u=0;u<BLOCK_SIZE;u++){
                L[transpose(i)+k*N+u] = L_BLO[i][k*BLOCK_SIZE+u];
                U[transpose(i)+k*N+u] = U_BLO[i][k*BLOCK_SIZE+u];
            }
        }
    }

    printf("L : \n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++)
            printf("%.4e ", L[i*N+j]);
        printf("\n");
    }
    printf("U : \n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++)
            printf("%.4e ", U[i*N+j]);
        printf("\n");
    }
    printf("A : \n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++)
            printf("%.4e ", A[i*N+j]);
        printf("\n");
    }
}

void copy(int index, double *copied, double *copy)
{
    int start = transpose(index);
    int dest = start + N*(BLOCK_SIZE-1);
    int cnt = 0;
    for(int i=start; i<=dest; i+=N)
        for(int j=0; j<BLOCK_SIZE; j++)
            copied[cnt++] = copy[i+j];
}
int transpose(int i) 
{
    return SQU_BLO*NUM_OF_BLOCK*(i/NUM_OF_BLOCK)+(i%NUM_OF_BLOCK)*BLOCK_SIZE;
}
