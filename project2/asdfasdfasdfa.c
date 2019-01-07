#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_OF_BLOCK 32

typedef struct index{
    int u_start;
    int u_end;
    int d_start;
    int d_end;
    int complete;
    int local_head;
}INDEX;

INDEX *arr;
double *C, *L, *A, *U, *A_temp;
int BLOCK_SIZE, N_, N, SEED, T, FLAG;
int bin_head, last;

double *Upper_Inverse();
double *Lower_Inverse();

void flow();
void mul_l();
void mul_u();
void print();
void mul();
void realprint();
void copy(int, double *, double *);
void LU_DE(int);
int main(int argc, char *argv[])
{
//    printf("=================================START=================================\n");
	if(argc != 5) {
		printf("Error Usage : <./project2 Size Threads>\n");
		exit(EXIT_FAILURE);
	}
	N_ = atoi(argv[1]);
    SEED = atoi(argv[2]);
	T = atoi(argv[3]);
    FLAG = atoi(argv[4]);    
	omp_set_num_threads(T);
   
    N = N_;
    if(N%NUM_OF_BLOCK)
        N += NUM_OF_BLOCK - N%NUM_OF_BLOCK;
    bin_head = (NUM_OF_BLOCK*NUM_OF_BLOCK)-(NUM_OF_BLOCK-1)*(NUM_OF_BLOCK-1)-1;

    A = (double *)calloc(N*N,sizeof(double));
	C = (double *)calloc(N*N,sizeof(double));
	L = (double *)calloc(N*N,sizeof(double));
    U = (double *)calloc(N*N,sizeof(double));
    
    srand(SEED);
    for(int i=0;i<N_;i++) 
		for(int j=0;j<N_;j++) {
			A[i*N+j] = (double)rand();
            C[i*N+j] = A[i*N+j];
			if(i==j)
				L[i*N+j] = 1.0d;
		}	
    for(int i=N_;i<N;i++){
        A[i*N+i] = 1.0d;
        L[i*N+i] = 1.0d;
    }

    BLOCK_SIZE = N/NUM_OF_BLOCK;
	arr = (INDEX *)calloc((N*N)/(BLOCK_SIZE*BLOCK_SIZE),sizeof(INDEX)); 
//    printf("N_ : %d, N : %d, B : %d \n",N_,N,BLOCK_SIZE); 
    for(int i=0; i<(N*N)/(BLOCK_SIZE*BLOCK_SIZE); i++) {
        arr[i].u_start = (BLOCK_SIZE)*(i%(N/BLOCK_SIZE)) + (BLOCK_SIZE)*N*(i/(N/BLOCK_SIZE));
        arr[i].u_end = arr[i].u_start + BLOCK_SIZE;
        arr[i].d_start = arr[i].u_start + (BLOCK_SIZE-1)*(N);
        arr[i].d_end = arr[i].d_start + BLOCK_SIZE;
        arr[i].complete = 0;
        //printf("세팅값 : %d %d %d %d %d\n", i, arr[i].u_start,arr[i].u_end,arr[i].d_start,arr[i].d_end);
    }
//    arr[3].complete = 1;
    for(int i=0;i<(N*N)/(BLOCK_SIZE*BLOCK_SIZE);i+=NUM_OF_BLOCK+1) {
        arr[i].local_head = bin_head;
        bin_head -= 2;
        last++;
        if(arr[i].complete == 0)
            flow(i);
    }

    realprint();
//    printf("==================================END==================================\n");
    return 0;
}

void flow(int index)
{
//    printf("기준 : %d, 팔 개수 : %d \n",index,arr[index].local_head);
    //printf("index : %d, local : %d-------",index,arr[index].local_head); 
    LU_DE(index);
//    printf("LU-DE 한 후 : %d\n",last);
//    print();
//    printf("\n");
    //Copy(index, Copied, Copying)
    double * L_temp = (double*)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    copy(index, L_temp, L);
    L_temp = Lower_Inverse(L_temp);

    double * U_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    copy(index, U_temp, U);
    U_temp = Upper_Inverse(U_temp);
    
    double *A_L_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    int local_head = arr[index].local_head/2;
    for(int i=1; i<=local_head; i++) {
//        printf("Upper order : %d ", index+i);
        copy(index+i, A_L_temp, A);
        mul_u(index+i, L_temp, A_L_temp);
    }
    if(arr[index].local_head != 0) {
    double *A_U_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    int num_temp = NUM_OF_BLOCK;
    for(int i=1; i<=local_head; i++) {
//        printf("Lower order : %d ", index+num_temp);
        copy(index+num_temp, A_U_temp, A);      
        mul_l(index+num_temp, A_U_temp, U_temp);
        num_temp+=NUM_OF_BLOCK;
    }
//    printf("\n");

//    printf("팔 구한 후 : \n");
//    print();
//    printf("\n");

    for(int i=1; i<=local_head; i++)
        for(int j=1; j<=local_head; j++)
            mul(index+(i*NUM_OF_BLOCK), j);
    
//    printf("mul한 후 : \n");
//    print();
//    printf("\n");
    }
    arr[index].complete = 1;
//    printf("\n");
}

void copy(int index, double *copied, double *copy)
{
    int start = arr[index].u_start;
    int dest = arr[index].d_start;
    int cnt = 0;
    for(int i=start; i<=dest; i+=N) {
        for(int j=0; j<BLOCK_SIZE; j++) {
            copied[cnt++] = copy[i+j];
        }
    }
}

void mul(int n, int m)
{
    int m_ = m;
    int n_ = n;
    n = n - last + 1;
    m = m + (last-1)*(NUM_OF_BLOCK+1); 
//    printf("mul에서의 last : %d, n_m_ :<%d, %d>,  nm : <%d, %d>, n : <%d,%d>, m : <%d,%d> <%d> \n", last, n_, m_,  n, m, arr[n].u_start, arr[n].d_start, arr[m].u_start, arr[m].d_start, arr[m].local_head); 
//    printf("mul 들어온 시점 : L[%d], U[%d]를 A[%d]에\n", n_, m, n_+m_);
//    print();
//    printf("\n");
    double *L_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double *U_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double *S_temp = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
       
    copy(n_, L_temp, L); 
    copy(m, U_temp, U);  
//    printf("L : %lf U : %lf\n",L_temp[0], U_temp[0]);
    double sum = 0.0d;
    for(int i=0; i<BLOCK_SIZE; i++){
        for(int j=0; j<BLOCK_SIZE; j++){
            sum = 0.0d;
            for(int k=0; k<BLOCK_SIZE; k++){
                sum += L_temp[i*BLOCK_SIZE+k]*U_temp[k*BLOCK_SIZE+j];
            }
            S_temp[i*BLOCK_SIZE+j] = sum;
        }
    }    
    int cnt = 0;
    for(int i=arr[m_+n_].u_start; i<=arr[m_+n_].d_start; i+=N){
        for(int j=0; j<BLOCK_SIZE; j++){
//            printf("/A[i+j] : %lf, S_temp[cnt] :  %lf %d %d/\n",A[i+j], S_temp[cnt], i+j, cnt);
            A[i+j] -= S_temp[cnt++];
        }
    }
}
                
void mul_u(int index, double *L_inv, double *A_temp)
{
    double *U_temp = (double*)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double sum = 0.0d;
    for(int i=0;i<BLOCK_SIZE;i++) {
        for(int j=0;j<BLOCK_SIZE;j++) {
            sum = 0.0d;
            for(int k=0;k<BLOCK_SIZE;k++) {
                sum += L_inv[i*BLOCK_SIZE+k]*A_temp[k*BLOCK_SIZE+j];
            }
            U_temp[i*BLOCK_SIZE+j] = sum;
        }
    }
    int cnt = 0;
    for(int i=arr[index].u_start;i<=arr[index].d_start;i+=N) {
        for(int j=0;j<BLOCK_SIZE;j++) {
            U[i+j]=U_temp[cnt++];
        }
    }
    free(U_temp);
}

void mul_l(int index, double *A_temp, double *U_inv)
{
/*    printf("---------------\n");
    for(int i=0;i<BLOCK_SIZE;i++){
        for(int j=0;j<BLOCK_SIZE;j++){
            printf("[%lf %lf] ",A_temp[i*BLOCK_SIZE+j],U_inv[i*BLOCK_SIZE+j]);
        }
        printf("\n");
    }
    printf("---------------\n");*/
    double *L_temp = (double*)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double sum = 0.0d;
    for(int i=0;i<BLOCK_SIZE;i++) {
        for(int j=0;j<BLOCK_SIZE;j++) {
            sum = 0.0d;
            for(int k=0;k<BLOCK_SIZE;k++) {
                sum += A_temp[i*BLOCK_SIZE+k]*U_inv[k*BLOCK_SIZE+j];
            }
            L_temp[i*BLOCK_SIZE+j] = sum;
        }
    }
    int cnt = 0;
    for(int i=arr[index].u_start;i<=arr[index].d_start;i+=N) {
        for(int j=0;j<BLOCK_SIZE;j++)
            L[i+j]=L_temp[cnt++];
    }
    free(L_temp);
}

double* Upper_Inverse(double *upper)
{
    double *rev = (double*)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double sum = 0.0d;
    /*for(int i = 0; i < BLOCK_SIZE; i++){
        rev[i*BLOCK_SIZE+i] = 1.0d / upper[i*BLOCK_SIZE+i];
        for(int j=BLOCK_SIZE-i-1; j>=0; j--){
            sum = 0.0d;
            for(int k = j; k <= i+j; k++)
                sum += upper[j*BLOCK_SIZE+k]*rev[k*BLOCK_SIZE+i+j];
            rev[j*BLOCK_SIZE+i+j] = -sum / upper[(i+j)*BLOCK_SIZE+i+j];
        }
    } */   
    for(int i = 0; i < BLOCK_SIZE; i++){
        rev[i*BLOCK_SIZE+i] = 1.0d / upper[i*BLOCK_SIZE+i];
        for(int j = i - 1; j >= 0; j--){
            sum = 0.0d;
            for(int k = j + 1; k <= i; k++)
                sum += upper[j*BLOCK_SIZE+k]*rev[k*BLOCK_SIZE+i];
            rev[j*BLOCK_SIZE+i] = -sum / upper[j*BLOCK_SIZE+j];
        }
    }
    return rev;
}

double* Lower_Inverse(double *lower)
{
    double *rev = (double *)malloc(sizeof(double)*BLOCK_SIZE*BLOCK_SIZE);
    double sum = 0.0d;

    for(int i=0;i<BLOCK_SIZE;i++) {
        rev[i*BLOCK_SIZE+i] = 1.0d;
        for(int j=0;j<i;j++){
            for(int k=0;k<i;k++)
                sum -= rev[k*BLOCK_SIZE+j] * lower[i*BLOCK_SIZE+k]/lower[k*BLOCK_SIZE+k];
            rev[i*BLOCK_SIZE+j] = sum;
            sum = 0.0d;
        }
    }
    return rev;
}

void LU_DE(int index) 
{
    int start = arr[index].u_start;
    int dest = arr[index].d_start;

    double *A_0 = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double *L_0 = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));
    double *U_0 = (double *)calloc(BLOCK_SIZE*BLOCK_SIZE, sizeof(double));

    int cnt = 0;
    for(int i=start; i<=dest; i+=N) {
        for(int j=0; j<BLOCK_SIZE; j++) {
            A_0[cnt] = A[i+j];
            L_0[cnt] = L[i+j];
            U_0[cnt++] = U[i+j];
        }
    }
    //printf("hihihi : %d--", cnt);
    for(int k=0;k<BLOCK_SIZE;k++) {
		U_0[k*BLOCK_SIZE+k] = A_0[k*BLOCK_SIZE+k];
        for(int i=k+1;i<BLOCK_SIZE;i++) {
			L_0[i*BLOCK_SIZE+k] = A_0[i*BLOCK_SIZE+k]/U_0[k*BLOCK_SIZE+k];
			U_0[k*BLOCK_SIZE+i] = A_0[k*BLOCK_SIZE+i];
		}
		for(int i=k+1;i<BLOCK_SIZE;i++)
			for(int j=k+1;j<BLOCK_SIZE;j++)
				A_0[i*BLOCK_SIZE+j]-=L_0[i*BLOCK_SIZE+k]*U_0[k*BLOCK_SIZE+j];
	}

    cnt = 0;
    for(int i=start; i<=dest; i+=N) {
        for(int j=0;j<BLOCK_SIZE;j++) {
            L[i+j] = L_0[cnt];
            U[i+j] = U_0[cnt++];
        }
    }
}

void realprint()
{
    printf("A : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++)
            printf("%.4e ", C[i*N+j]);        
        printf("\n");
    }
    printf("L : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++) 
            printf("%.4e ", L[i*N+j]);     
    }
    printf("U : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++)
            printf("%.4e ", U[i*N+j]);
    }
}

void print()
{
    printf("A : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++)
            printf("%lf ", A[i*N+j]);        
        printf("\n");
    }
    printf("L : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++) 
            printf("%lf ", L[i*N+j]);     
        printf("\n");
    }
    printf("U : \n");
    for(int i=0;i<N_;i++) {
        for(int j=0;j<N_;j++)
            printf("%lf ", U[i*N+j]);
        printf("\n");
    }
}
