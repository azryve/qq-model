#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "gsl/gsl.h"


#define STATE_SIZE 256

typedef  double timings_t;
typedef timings_t* timev_t;


int calculate_wait_vec(timev_t qt, timev_t st, timev_t wt, size_t size){
	size_t i;
	wt[0] = 0;
	double u;
	for(i=0; i<size-1; ++i){
		wt[i+1] = ((u = wt[i] + st[i] - qt[i+1])>0?u:0);
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

void print_usage(){
	printf("model_new <av> <util> <size>\n");
}

int main(int argc, char* argv[]){
	if(argc < 3) {
		print_usage();
		exit(1);
	}
	av = (timings_t) strtod(argv[1], 0);
	util = (timings_t) strtod(argv[2], 0);
	size = (size_t) strtoul(argv[3], 0, 10);

	timev_t q_vec = malloc(size * sizeof(timings_t));
	timev_t s_vec = malloc(size * sizeof(timings_t));
	timev_t w_vec = malloc(size * sizeof(timings_t));

	struct distr_param_s dp;
	dp.type=exponential;
	dp.mean=(double) av;
	rand_v((double*) q_vec, size, dp);

	dp.mean*=util;
	dp.type=erlang;
	dp.g_form=0.5;
	dp.g_mult=dp.mean/dp.g_form;
	rand_v((double*) s_vec, size, dp);
	
	calculate_wait_vec(q_vec, s_vec, w_vec, size);
	
	printf("%f\n", ave_vec(q_vec, size));
	printf("%f\n", ave_vec(s_vec, size));
	printf("%f\n", ave_vec(w_vec, size));

	return 0;
}
