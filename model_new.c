#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "gsl/gsl.h"

#include <getopt.h>

// ?? old stuff ?? #define STATE_SIZE 256

typedef  double timings_t;
typedef timings_t* timev_t;


int calculate_wait_vec(timev_t qt, timev_t st, timev_t wt, size_t size){
	size_t i;
	wt[0] = 0;
	double u;

	for(i=0; i<size-1; ++i){
		wt[i+1] = ((u = wt[i] + st[i] - qt[i+1])>0 ? u : 0);  /* wait_time of first query is 0
									 wait_time of N+1th query wt[i+1] is how long
									 + wt[i] Nth query waited  
									 + st[i] Nth query had been processed
									 - qt[i+1] time passed before N+1th arrived  
								      */
	}
	return 0;
}

double ave_vec(double *vec, size_t size){
	size_t i=0;
	double av=0;

	while(i<size){ av += vec[i++]/size; }
	return av;
}

size_t size = 10000;
timings_t av = 1;
double util = 0.5;

void print_usage(const char *filename){
	printf("%s [-f] -a <av> -u <util> -s <size>\n", filename);
}

int main(int argc, char* argv[]){
	int oflag=0;
	int rc=0;

	char opt;
	while((opt=getopt(argc, argv, "fu:a:s:")) != -1){
		switch(opt){
			case('f'):
				oflag=1;
				break;
			case('u'):
				util = (timings_t) strtod(argv[2], 0);
				break;
			case('a'):
				av = (timings_t) strtod(optarg, 0);
				break;
			case('s'):
				size = (size_t) strtoul(argv[3], 0, 10);
				break;

			default:
				print_usage(argv[0]);
				rc=1;
				goto end;
		}
	}

	timev_t q_vec = malloc(size * sizeof(timings_t));
	timev_t s_vec = malloc(size * sizeof(timings_t));
	timev_t w_vec = malloc(size * sizeof(timings_t));

	struct distr_param_s dp;
	dp.type=exponential;
	dp.mean=(double) av;
	rand_v((double*) q_vec, size, dp);	/*generating query intervals*/

	dp.mean*=util;
	dp.type=erlang;
	dp.g_form=0.5;
	dp.g_mult=dp.mean/dp.g_form;
	rand_v((double*) s_vec, size, dp);	/*generatig serving intervals*/
	
	calculate_wait_vec(q_vec, s_vec, w_vec, size);
	
	printf("%f\n", ave_vec(q_vec, size));
	printf("%f\n", ave_vec(s_vec, size));
	printf("%f\n", ave_vec(w_vec, size));

	if(oflag) {
		printf("oflag\n");
	}

	end:
		return rc;
}
