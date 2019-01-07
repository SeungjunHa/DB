#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define SINGULAR_MATRIX_ERROR do{\
	printf("Sigular Matrix Error\n");\
	exit(-1);\
}while(0)\

#define SWAP(a,b) do{\
	double temp;\
	temp = a;\
	a = b;\
	b = temp;\
}while(0)

#define SWAP_INT(a,b) do{\
	int temp;\
	temp = a;\
	a = b;\
	b = temp;\
}while(0)

#define EPSILON 0.000001

double *C, *A, *L, *U, *S, *U_S;
int *P;
int N, T, SEED, FLAG;

void randon_generalization();
void total_malloc();
void total_free();
void print_matrix();

void random_generalization()
{
    srand(SEED);
	omp_set_num_threads(T);
//#pragma omp parallel for schedule(auto)
	for(int i=0;i<N;i++) {
		for(int j=0;j<N;j++) {
			A[i*N+j] = rand();
            C[i*N+j] = A[i*N+j];
			if(i==j)
				L[i*N+j] = 1.0f;
		}
	}
}

int main(int argc, char *argv[])
{
	double Rand_start = omp_get_wtime();
	if(argc != 5) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
	N = atoi(argv[1]);
    SEED = atoi(argv[2]);
	T = atoi(argv[3]);
    FLAG = atoi(argv[4]);
    
	total_malloc();
	random_generalization();
	double Rand_time = omp_get_wtime()-Rand_start;
//	printf("Random Time : %lf sec.\n", Rand_time);
	double Gaussian_start = omp_get_wtime();
	double max;
	double tmp;
	int k = 0, k_ = 0, i = 0;	
	for(i=0;i<N;i++)
		P[i] = i;

	for(k=0;k<N;k++) {
		max = 0.0f;
		for(i=k;i<N;i++) {
			tmp = A[i*N+k];
			if(A[i*N+k] < 0)
				tmp = -A[i*N+k];
			if(max < tmp) {
				max = tmp;
				k_ = i;
			}
		}

		if(max == 0)
			SINGULAR_MATRIX_ERROR;

		SWAP_INT(P[k], P[k_]);		
		for(i=0;i<=N-1;i++)
			SWAP(A[k*N+i], A[k_*N+i]);
		for(i=0;i<=k-1;i++)
			SWAP(L[k*N+i], L[k_*N+i]);

		U[k*N+k] = A[k*N+k];

		for(i=k+1;i<N;i++) {
			L[i*N+k] = A[i*N+k]/U[k*N+k];
			U[k*N+i] = A[k*N+i];
		}
#pragma omp parallel for schedule(auto)
		for(int i=k+1;i<N;i++) {
			for(int j=k+1;j<N;j++) {
				A[i*N+j] = A[i*N+j] - L[i*N+k]*U[k*N+j];
			}
		}
	}
	double Gaussian_time = omp_get_wtime()-Gaussian_start;
//	printf("Gaussian Time : %lf sec.\n", Gaussian_time);
    /*if(FLAG == 1)
        print_matrix();
    else
        return 0;*/

	double Check_start = omp_get_wtime();

//	double swap = 0.0f;
//#pragma omp parallel for private(sum) schedule(auto)
#pragma omp parallel for schedule(auto)
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++) {
			U_S[i*N+j] = U[j*N+i];
			/*swap = U[j*N+i];
			U[j*N+i] = U[i*N+j];
			U[i*N+j] = swap;*/
		}

	for(int i=0;i<N;i++) {
#pragma omp parallel for schedule(auto)
		for(int j=0;j<N;j++) {
			for(int k=0;k<N;k++)
				//S[i*N+j] += L[i*N+k]*U[k*N+j];
				S[i*N+j] += L[i*N+k]*U_S[j*N+k];
			//S[i][j]=sum;
			//sum = 0.0f;
		}
	}

#pragma omp parallel for schedule(auto)
	for(int i=0;i<N;i++) {
		for(int j=0;j<N;j++)
			if(fabs(C[P[i]*N+j]-S[i*N+j]) >  EPSILON) {
				printf("[%d %d],Wrong Answer\n", P[i], j);
				print_matrix();
			}
	}
    print_matrix();
	double Check_time = omp_get_wtime()-Check_start;
//	printf("Check Time : %lf sec.\n", Check_time);

	printf("Test1 Total Time : %lf, [%lf + %lf + %lf] sec.\n", Check_time+Gaussian_time+Rand_time, Rand_time, Gaussian_time, Check_time);
	total_free();
	return 0;
}

void total_malloc()
{
	C = (double *)calloc(N*N,sizeof(double));
	A = (double *)calloc(N*N,sizeof(double));
	L = (double *)calloc(N*N,sizeof(double));
	U = (double *)calloc(N*N,sizeof(double));
	U_S = (double *)calloc(N*N,sizeof(double));
	S = (double *)calloc(N*N,sizeof(double));
	P = (int *)calloc(N, sizeof(int));
}

void total_free()
{
	if(P != NULL)
		free(P);
	if(A != NULL)	
		free(A);	
	if(L != NULL)
		free(L);
	if(U != NULL)
		free(U);
	if(C != NULL)
		free(C);
	if(S != NULL)
		free(S);
}

void print_matrix()
{
	int i,j;
	printf("Array P\n");
	for(i=0;i<N;i++)
		printf("%d\n",P[i]);
	printf("\n");
	printf("Array L\n");
	for(i=0;i<N;i++) {
		for(int j=0;j<N;j++)
			printf("%.4e ",L[i*N+j]);
		printf("\n");
	}
	printf("\n");
	printf("Array U\n");
	for(i=0;i<N;i++) {
		for(int j=0;j<N;j++)
			printf("%.4e ",U[i*N+j]);
		printf("\n");
	}
	printf("\n");
	printf("Array C\n");
	for(i=0;i<N;i++) {
		for(j=0;j<N;j++)
			printf("%.4e ",C[i*N+j]);
		printf("\n");
	}
	printf("\n");
	printf("Array S\n");
	for(i=0;i<N;i++) {
		for(j=0;j<N;j++)
			printf("%.4e ",S[i*N+j]);
		printf("\n");
	}
	printf("\n");
}
