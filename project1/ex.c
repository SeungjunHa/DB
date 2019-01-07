#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

// function prototypes
void update(long number);

/*
    * update global aggregate variables given a number
     */
void update(long number)
{
	    // simulate computation
	    sleep(number);

	        // update aggregate variables
	        sum += number;
		    if (number % 2 == 1) {
			            odd++;
				        }
		        if (number < min) {
				        min = number;
					    }
			    if (number > max) {
				            max = number;
					        }
}

int main(int argc, char* argv[])
{
	clock_t start, stop;
	double cputime;
	start = clock();
	    // check and parse command line options
	    if (argc != 2) {
		            printf("Usage: sum <infile>\n");
			            exit(EXIT_FAILURE);
				        }
	        char *fn = argv[1];

		    // load numbers and add them to the queue
		    FILE* fin = fopen(fn, "r");
		        char action;
			    long num;
			        while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
					        if (action == 'p') {            // process
							            update(num);
								            } else if (action == 'w') {     // wait
										                sleep(num);
												        } else {
														            printf("ERROR: Unrecognized action: '%c'\n", action);
															                exit(EXIT_FAILURE);
																	        }
						    }
				    fclose(fin);

				    stop = clock();
				    cputime = (double)(stop - start)/CLOCKS_PER_SEC;
				        // print results
				        printf("%ld %ld %ld %ld %f\n", sum, odd, min, max, cputime);

					    // clean up and return
					    return (EXIT_SUCCESS);
}
