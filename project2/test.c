#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define SINGULAR_MATRIX_ERROR do{\
	printf("Sigular Matrix Error\n");\
	exit(-1);\
}while(0)\

#define SWAP(a, b) do{\
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

double **C, **A, **L, **U, **S;
int *P;
int N, T;

void randon_generalization();
void total_malloc();
void total_free();
void print_matrix();

void random_generalization()
{
	struct drand48_data Rand_Buffer;
	srand48_r(time(NULL)%3123124124, &Rand_Buffer);

	omp_set_num_threads(T);
#pragma omp parallel for schedule(auto)
	for(int i=0;i<N;i++) {
		for(int j=0;j<N;j++) {
			drand48_r(&Rand_Buffer, &A[i][j]);
			A[i][j] = A[i][j]*100.0f - 50.0f  + 0.0001f;
			C[i][j] = A[i][j];
			if(i==j)
				L[i][j] = 1.0f;
		}
	}
}

int main(int argc, char *argv[])
{
	double Rand_start = omp_get_wtime();
	if(argc != 3) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
	N = atoi(argv[1]);
	T = atoi(argv[2]);

	total_malloc();
	random_generalization();
	double Rand_time = omp_get_wtime()-Rand_start;
//	printf("Random Time : %lf sec.\n", Rand_time);

	double Gaussian_start = omp_get_wtime();
	double max;
	double tmp;
	double *swa;
	int k = 0, k_ = 0, i = 0, j = 0;	
	for(i=0;i<N;i++)
		P[i] = i;

	for(k=0;k<N;k++) {
		max = 0.0f;
		for(i=k;i<N;i++) {
			tmp = A[i][k];
			if(A[i][k] < 0)
				tmp = -A[i][k];
			if(max < tmp) {
				max = tmp;
				k_ = i;
			}
		}

		if(max == 0)
			SINGULAR_MATRIX_ERROR;

		SWAP_INT(P[k], P[k_]);		
		
		swa = A[k];
		A[k] = A[k_];
		A[k_] = swa;

		for(i=0;i<=k-1;i++)
			SWAP(L[k][i], L[k_][i]);

		U[k][k] = A[k][k];

		for(i=k+1;i<N;i++) {
			L[i][k] = A[i][k]/U[k][k];
			U[k][i] = A[k][i];
		}
#pragma omp parallel for private(j) schedule(auto)
		for(i=k+1;i<N;i++) {
			for(j=k+1;j<N;j++) {
				A[i][j] = A[i][j] - L[i][k]*U[k][j];
			}
		}
	}
	double Gaussian_time = omp_get_wtime()-Gaussian_start;
//	printf("Gaussian Time : %lf sec.\n", Gaussian_time);

	double Check_start = omp_get_wtime();
	S = (double **)malloc(sizeof(double *)*N);
	for(i=0;i<N;i++)
		S[i] = (double *)calloc(N, sizeof(double));

	//double sum = 0.0f;
	for(int i=0;i<N;i++) {
#pragma omp parallel for schedule(auto)
		for(int j=0;j<N;j++) {
			for(int k=0;k<N;k++)
				S[i][j] += L[i][k]*U[k][j];
			//S[i][j]=sum;
			//sum = 0.0f;
		}
	}

#pragma omp parallel for private(j) schedule(auto)
	for(i=0;i<N;i++) {
		for(j=0;j<N;j++)
			if(fabs(C[P[i]][j]-S[i][j]) >  EPSILON) {
				printf("[%d %d],Wrong Answer\n", P[i], j);
				print_matrix();
			}
	}
	double Check_time = omp_get_wtime()-Check_start;
//	printf("Check Time : %lf sec.\n", Check_time);

	printf("Total Time : %lf, [%lf + %lf + %lf] sec.\n", Check_time+Gaussian_time+Rand_time, Rand_time, Gaussian_time, Check_time);
	total_free();
	return 0;
}

void total_malloc()
{
	C = (double **)malloc(sizeof(double *)*N);
	A = (double **)malloc(sizeof(double *)*N);
	L = (double **)malloc(sizeof(double *)*N);
	U = (double **)malloc(sizeof(double *)*N);
	P = (int *)calloc(N, sizeof(int));
	for(int i=0;i<N;i++) {
		C[i] = (double *)calloc(N, sizeof(double));
		A[i] = (double *)calloc(N, sizeof(double));
		L[i] = (double *)calloc(N, sizeof(double));
		U[i] = (double *)calloc(N, sizeof(double));
	}
}

void total_free()
{
	for(int i=0;i<N;i++) {
		if(A[i] != NULL)
			free(A[i]);
		if(L[i] != NULL)
			free(L[i]);
		if(U[i] != NULL)
			free(U[i]);
		if(C[i] != NULL)
			free(C[i]);
	}
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
			printf("%lf ",L[i][j]);
		printf("\n");
	}
	printf("\n");
	printf("Array U\n");
	for(i=0;i<N;i++) {
		for(int j=0;j<N;j++)
			printf("%lf ",U[i][j]);
		printf("\n");
	}
	printf("\n");
	printf("Array C\n");
	for(i=0;i<N;i++) {
		for(j=0;j<N;j++)
			printf("%lf ",C[P[i]][j]);
		printf("\n");
	}
	printf("\n");
	printf("Array S\n");
	for(i=0;i<N;i++) {
		for(j=0;j<N;j++)
			printf("%lf ",S[i][j]);
		printf("\n");
	}
	printf("\n");
}
