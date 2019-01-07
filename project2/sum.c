#include <stdio.h>
#include <omp.h>
#include <limits.h>

int sum = INT_MIN;
int main(void)
{
	double start = omp_get_wtime();
		
#pragma omp parallel for num_threads(4)
	for(int i=0;i<(INT_MAX>>7);i++) {
		sum++;
	}
	sum+=1;
	printf("%lf \n", omp_get_wtime()-start);

	return 0;
}
