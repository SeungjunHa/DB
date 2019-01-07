# include <iostream>
# include <omp.h>
# include <stdlib.h>
# include <vector>
# include <ctime>

std::vector<std::vector<double> > matrix; // N*N matrix
std::vector<std::vector<double> > lower; // L matrix
std::vector<std::vector<double> > upper;// U matrix
std::vector<int> perm; // permutation matrix
std::vector<std::vector<double> > dupli; // N*N matrix

void Swap_(std::vector<int> & perm, int k, int max_idx){
	double temp = perm[k];
	perm[k]=perm[max_idx];
	perm[max_idx]=temp;
}

void Swap(std::vector<std::vector<double> > & matrix, int k, int max_idx, int flag){
	if(flag==1){	// swap a(k,:) and a(max_idx, :)
		std::vector<double> temp = matrix[k];
		matrix[k]=matrix[max_idx];
		matrix[max_idx]=temp;
	}
	else{	// swap l(k,1:k-1) and l(max_idx, 1:k-1)
		double temp;
		for(int i=1; i<=k-1; i++){
			temp = matrix[k][i];
			matrix[k][i]=matrix[max_idx][i];
			matrix[max_idx][i]=temp;
		}
	}
} 
void Seri(int start, int end, int num_t){
	double global_max=0;
	int max_idx=0;
	for(int k=start; k<=end; k++){ // 1~10 , 11~20, 21~30, ...
		#pragma omp parallel num_threads(num_t) shared(k, matrix, upper, lower)
		{
		// Find the maximum element
	/*	double max_value=0;
		int max_idx=0;
		#pragma omp parallel num_threads(num_t) 
		{
			double local_max=0;
			int idx=0;
			#pragma omp for reduction(max : max_value) 
			for(int i=k; i<=end; i++){
				if(max_value < dupli[i][k]){
					max_value=dupli[i][k];
					local_max=dupli[i][k];
					idx=i;	
				}
			}
			#pragma omp critical
			{
				if(local_max == max_value)
					max_idx=idx;		
			}	*/	
			#pragma omp single
			upper[k][k]=dupli[k][k];
			#pragma omp for
			for(int j=k+1; j<=end; j++){
				lower[j][k] = dupli[j][k] / upper[k][k];
				upper[k][j] = dupli[k][j];
			}
			#pragma omp for
			for(int i=k+1; i<=end; i++){
				for(int j=k+1; j<=end; j++){
					dupli[i][j]=dupli[i][j]-lower[i][k]*upper[k][j];
				}
			}
		}// Parallel 
	} // k= start ~ end
}
void forward_subs(int start, int end, int num_t, int N){// step 2 : compute (A01 = L00 * UO1) by parallelized forward substitution
	// LOO = row(start~end) col(start~end)  U01 = row(start~end) col(start~N) A01 = row(start~end) col(start~N)
	#pragma omp parallel num_threads(num_t)
	{
		#pragma omp for
		for(int col= end+1; col <=N; col++){
			for(int row=start; row<=end; row++){
				upper[row][col] = dupli[row][col] / lower[row][row]; 
				for(int i_row=row+1; i_row<=end; i_row++){
					dupli[i_row][col] = dupli[i_row][col] - upper[row][col] * lower[i_row][row];
				}	
			}
		}
	}
}
void backward_subs(int start, int end, int num_t, int N){
	// step 3 : compute (A10 = L10 * U00) by parallelized back substitution
	// L10 = row(end~N) col(start~end) U00 = row(start~end) col(start~end) A10 = row(end~N) col(start~end)	
	#pragma omp parallel num_threads(num_t)
	{
		#pragma omp for
		for(int row = end+1; row<=N; row++){
			for(int col = start; col <= end; col++){
				lower[row][col] = dupli[row][col] / upper[col][col]; 
				for(int i_col = col+1; i_col<=end; i_col++){
					dupli[row][i_col] = dupli[row][i_col] - upper[col][i_col] * lower[row][col] ;
				}
			} 
		}
	}
}

void Paral(int N, int num_t){
	int recur = 100;		// size of b
	int first = 0;
	 dupli = matrix;
	if(N <= recur){
		int start = 1;
		int end = N;	
		Seri(start, end, num_t);
	}
	else{
		while(recur <= N){
			// step 1 : Compute (A^00 = L^00 * U^00) by serial LU factorization
			// A^00 = (10 * 10)	L^00 = (10 * 10)	U^00 = (10 * 10)	>> get L00 U00
			int start = recur - 99;
			int end;
			if(N-recur <=100){// Last step
				end = N;
				Seri(start, end, num_t);
			}
			else{
				end = recur;
				Seri(start, end ,num_t); // (1~10) (11~20) (21~30) ....	
				#pragma omp parallel num_threads(num_t) 
				{
					#pragma omp sections
					{
					// step 2 : compute (A01 = L00 * UO1) by parallelized forward substitution >> get U01
					#pragma omp section
					forward_subs(start, end, num_t/2, N);
					// step 3 : compute (A10 = L10 * U00) by parallelized back substitution >> get L10
					#pragma omp section
					backward_subs(start, end, num_t/2, N);
					}
					// step 4 : compute (L11 * U11 = A11 - L10 * U01) .. merely by matrix multiplication
					//recursively >> A11 , L11, U11
				}
			}
			recur+=100;
		}//while
	}
}
void Init(int N, int rseed){
	matrix.assign(N+1, std::vector<double>(N+1));
	upper.assign(N+1, std::vector<double>(N+1));
	lower.assign(N+1, std::vector<double>(N+1));
	perm.assign(N+1,0);
	// Initiailization
	srand((unsigned)rseed);
	for(int i=1; i<=N; i++){
		for(int j=1; j<=N; j++){
		//	matrix[i][j]=drand48();	
			matrix[i][j]=rand();
		}
	}
	for(int i=1; i<=N; i++){
		perm[i]=i;
	}
	for(int i=1; i<=N; i++){
		for(int j=1; j<=N; j++){
			if(i==j)
				lower[i][j]=1;
			else
				lower[i][j]=0;
		}
	}
	for(int i=1; i<=N; i++){
		for(int j=1; j<=N; j++){
			upper[i][j]=0;
		}
	}
}
void Multiplication(std::vector<std::vector<double> > & pa_lu,
		    std::vector<std::vector<double> > & L,
		    std::vector<std::vector<double> > & U, int N){
	for(int i=1; i<=N; i++){
		for(int j=1; j<=N; j++){
			for(int k=1; k<=N; k++){
				pa_lu[i][j] += L[i][k] * U[k][j];
			}
		}
	}

}

int main(int argc, char * argv[]){
	if(argc !=5){
		std::cout << "Incorrect number of inputs" << std::endl;
		exit(-1);
	}	
	int N = strtol(argv[1], NULL, 10);	// N*N matrix
	int rseed = strtol(argv[2], NULL, 10);
	int num_t = strtol(argv[3], NULL, 10); // Number of threads
	int print_flag = strtol(argv[4], NULL, 10);
	Init(N, rseed);
	clock_t start;
	clock_t end;

	start = clock();
	std:: cout << "start clock : " << start << std::endl;
	Paral(N, num_t);
	end = clock();
	std:: cout << "end clock : " << end << std::endl;
	// PA - LU
	if(print_flag == 1){
		std::vector<std::vector<double> > pa_lu(N+1, std::vector<double>(N+1,0));
	//	for(int i=1; i<=N; i++){
	//		ans[i]=matrix[perm[i] ];
	//	}
		std::cout << "L:" << std::endl;
		for(int i=1; i<=N; i++){
			for(int j=1; j<=N; j++){
				std::cout << lower[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "U:" << std::endl;
		for(int i=1; i<=N; i++){
			for(int j=1; j<=N; j++){
				std::cout << upper[i][j] << " ";
			}
			std::cout<< std::endl;
		}	
		std::cout << "A:" << std::endl;
		for(int i=1; i<=N; i++){
			for(int j=1; j<=N; j++){
				std:: cout << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "PA-LU:" << std::endl;
		Multiplication(pa_lu, lower, upper, N);
		for(int i=1; i<=N; i++){
			for(int j=1; j<=N; j++){
				std::cout << pa_lu[i][j] - matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}	
	}
}
