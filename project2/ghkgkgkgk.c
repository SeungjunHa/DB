#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define BLOCK_SIZE 32
#define SQU_BLO 1024

double *A;
int N_, N;
int NUM_OF_BLOCK, TOTAL_NUM_OF_BLOCK;

void flow(double **, double **, double **, int, int, int, int)
void LU_DE(double *, double *, double *, int);
void mul(double *, double *, double *)
void mul_l(int, double *, double *, double *)
void mul_u(int, double *, double *, double *)
double *Upper_Inverse();
double *Lower_Inverse();
void print(double **, double **);
void copy(int, double *, double *);
int transpose(int);

int main(int argc, char *argv[])
{
	if(argc != 5) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
	N_ = atoi(argv[1]);
	omp_set_num_threads(atoi(argv[3]));    
    
    int padding;
    N_ = N;
    if(padding=(N%BLOCK_SIZE))
        N += BLOCK_SIZE-padding;
    NUM_OF_BLOCK = N/BLOCK_SIZE;
    TOTAL_NUM_OF_BLOCK = NUM_OF_BLOCK*NUM_OF_BLOCK;
    int NN = N*N;
    A = (double *)calloc(NN, sizeof(double))
    
    srand(atoi(argv[2]));
    for(int i=0;i<N_;i++)
		for(int j=0;j<N_;j++)
			A[i*N+j] = (double)rand();            
    for(int i=N_;i<N;i++)
        A[i*N+j] = 1.0d;

    double **A_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    double **L_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    double **U_BLOCK = (double **)malloc(sizeof(double *)*TOTAL_NUM_OF_BLOCK);
    for(int i=0;i<TOTAL_NUM_OF_BLOCK;i++) {
        A_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        l_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        U_BLOCK[i] = (double *)calloc(SQU_BLO, sizeof(double));
        copy(i, A_BLOCK[i], A);
    }
    
    double start = omp_get_wtime();
    
    int BODY;
    int STAGE = 1;
    int WING = NUM_OF_BLOCK;
    int temp = NN/SQU_BLO;
    for(int i=0; i<temp; i+=NUM_OF_BLOCK+1) {
        for(int j=0; j<1024; j+=BLOCK_SIZE+1)
            L[i][j] = 1.0d;
        WING--;
        BODY = WING*WING;
        flow(A_BLOCK, i, WING, BODY, STAGE);
        STAGE++;
    }
    printf("Time : %lf\n", omp_get_wtime()-start);

    if(!atoi(argv[5]))
        print(L_BLOCK, U_BLOCK);
    return 0;
}

void flow(double **A_BLOCK, double **L_BLOCK, double **U_BLOCK, int index, int WING, int BODY, int STAGE)
{
    LU_DE(A_BLOCK[index], L_BLOCK[index], U_BLOCK[index], index);
    
    double * L_inv = (double*)calloc(SQU_BLO, sizeof(double));
    L_inv = Lower_Inverse(L_BLOCK[index]);

    double * U_inv = (double *)calloc(SQU_BLO, sizeof(double));
    U_inv = Upper_Inverse(U_BLOCK[index]);
     
    for(int i=1; i<=WING; i++) {
        int simple_l = index+i*NUM_OF_BLOCK;
        mul_l(simple_l, A_BLOCK[simple_l], U_inv, L_BLOCK[simple_l]);
        int simple_u = index+i;
        mul_u(simple_u, L_inv, A_BLOCK[simple_u], U_BLOCK[simple_u]);
    }
     
    for(int i=1; i<=BODY; i++){
        int A_pos = index + NUM_OF_BLOCK + i + (i-1)*STAGE/WING;
        int temp = A_pos % NUM_OF_BLOCK;
        int L_pos = temp + (STAGE-1)*NUM_OF_BLOCK;
        int U_pos = A_pos - temp + (STAGE-1);

        mul(A_BLOCK[A_pos], L_BLOCK[L_pos], U_BLOCK[U_pos]);
    }
}

void LU_DE(double *A_BLOCK, double *L_BLOCK, double *U_BLOCK, int index) 
{
    int i_temp, k_temp;
    int i_temp_k, k_temp_i, k_temp_k;
    for(int k=0;k<BLOCK_SIZE;k++) {
        k_temp = k*BLOCK_SIZE;
        k_temp_k = k_temp + k;
		U_BLOCK[k_temp_k] = A_BLOCK[k_temp_k];
        for(int i=k+1;i<BLOCK_SIZE;i++) {
            i_temp = i*BLOCK_SIZE;
            i_temp_k = i_temp + k;
            k_temp_i = k_temp + i;
			L_BLOCK[i_temp_k] = A_BLOCK[i_temp_k]/U_BLOCK[k_temp+k];
			U_BLOCK[k_temp_i] = A_BLOCK[k_temp_i];
		}
		for(int i=k+1;i<BLOCK_SIZE;i++)
            i_temp = i*BLOCK_SIZE;
			for(int j=k+1;j<BLOCK_SIZE;j++)
				A_BLOCK[i_temp+j]-=L_BLOCK[i_temp+k]*U_BLOCK[k_temp+j];
	}
}

void copy(int index, double *copied, double *copy)
{
    int start = transpose(index);
    int dest = start + N*BLOCK_SIZE;
    int cnt = 0;
    for(int i=start; i<dest; i+=N)
        for(int j=0; j<BLOCK_SIZE; j++)
            copied[cnt++] = copy[i+j];
}

void mul(double *A_BLO, double *L_BLO, double *U_BLO)
{    
    double sum = 0.0d;
    int i_temp;
    for(int i=0; i<BLOCK_SIZE; i++) {
        i_temp = i*BLOCK_SIZE;
        for(int j=0; j<BLOCK_SIZE; j++){
            for(int k=0; k<BLOCK_SIZE; k++)
                sum += L_temp[i_temp+k]*U_temp[k*BLOCK_SIZE+j];
            A_BLO[i_temp+j] = sum;
            sum = 0.0d;
        }
    }
}
                
void mul_u(int pos, double *L_inv, double *A_BLO, double *U_BLO)
{   
    double sum = 0.0d;
    int i_temp;
    for(int i=0;i<BLOCK_SIZE;i++) {
        i_temp = i*BLOCK_SIZE;
        for(int j=0;j<BLOCK_SIZE;j++) {
            for(int k=0;k<BLOCK_SIZE;k++)
                sum += L_inv[i_temp+k]*A_BLO[k*BLOCK_SIZE+j];
            U_BLO[i_temp+j] = sum;
            sum = 0.0d;
        }
    }
}

void mul_l(int pos, double *A_BLO, double *U_inv, double *L_BLO)
{
    double sum = 0.0d;
    int i_temp;
    for(int i=0;i<BLOCK_SIZE;i++) {
        i_temp = i*BLOCK_SIZE;
        for(int j=0;j<BLOCK_SIZE;j++) { 
            for(int k=0;k<BLOCK_SIZE;k++)
                sum += A_BLO[i_temp+k]*U_inv[k*BLOCK_SIZE+j];
            L_BLO[i_temp+j] = sum;
            sum = 0.0d;
        }
    }
}

double* Upper_Inverse(double *upper)
{
    double *rev = (double*)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double sum = 0.0d;
    int i_temp;
    int j_temp;
    for(int i = 0; i < BLOCK_SIZE; i++){
        i_temp = i*BLOCK_SIZE + i;
        rev[i_temp] = 1.0d / upper[i_temp];
        for(int j = i - 1; j >= 0; j--){
            j_temp = j*BLOCK_SIZE;
            for(int k = j + 1; k <= i; k++)
                sum -= upper[j_temp+k]*rev[k*BLOCK_SIZE+i];
            rev[j_temp+i] = sum/upper[j_temp+j];
            sum = 0.0d;
        }
    }
    return rev;
}

double* Lower_Inverse(double *lower)
{
    double *rev = (double *)malloc(sizeof(double)*BLOCK_SIZE*BLOCK_SIZE);
    double sum = 0.0d;
    int i_temp;
    for(int i=0;i<BLOCK_SIZE;i++) {
        i_temp = i*BLOCK_SIZE;
        rev[i_temp+i] = 1.0d;
        for(int j=0;j<i;j++) {
            for(int k=0;k<i;k++)
                sum -= rev[k*BLOCK_SIZE+j]*lower[i_temp+k];
            rev[i_temp+j] = sum/lower[j*BLOCK_SIZE+j];
            sum = 0.0d;
        }
    }
    return rev;
}

void print(double **L_BLO, double **U_BLO)
{
    double *L = (double *)malloc(sizeof(double)*NUM_OF_BLOCK);
    double *U = (double *)malloc(sizeof(double)*NUM_OF_BLOCK);

    int i_temp;
    for(int i=0; i<NUM_OF_BLOCK; i++) {
        i_temp = i*BLOCK_SIZE;
        for(int j=0; j<NUM_OF_BLOCK; j++) 
            for(int k=i_temp; k<i_temp+BLOCK_SIZE; k++) {
                L[i] = L_BLO[j][k];
                U[i] = U_BLO[j][k];
            }
    }
    
    printf("L:\n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++) 
            printf("%.4e ", L[i*N+j]);
        printf("\n");
    }
    printf("U:\n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++)
            printf("%.4e ", U[i*N+j]);
        printf("%\n");
    }
    printf("A:\n");
    for(int i=0; i<N_; i++) {
        for(int j=0; j<N_; j++)
            printf("%.4e ", A[i*N+j]);
        printf("\n");
    }
}

void copy(int index, double *copied, double *copy)
{
    int start = transpose(index);
    int dest = start + N*BLOCK_SIZE;
    int cnt = 0;
    for(int i=start; i<dest; i+=N)
        for(int j=0; j<BLOCK_SIZE; j++)
            copied[cnt++] = copy[i+j];
}
int transpose(int i) {return BLOCK_SIZE*(i*NUM_OF_BLOCK+i*BLOCK_SIZE);}
