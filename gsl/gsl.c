#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <time.h>
#include "gsl.h"

int init_flag=0;
gsl_rng *R;

void init_rng(){
	const gsl_rng_type *T = gsl_rng_default;
	R = gsl_rng_alloc(T);
	gsl_rng_set(R, (unsigned long int) time(0));
	init_flag=1;
}

size_t rand_v(double *vec, size_t vec_size, struct distr_param_s distr_param){
	if(!init_flag){ init_rng(); }

	size_t i;
	for(i=0;i<vec_size;++i){
		switch(distr_param.type){
			case exponential:
				vec[i] = gsl_ran_exponential(R,distr_param.mean);
				break;
			case erlang:
				vec[i] = gsl_ran_gamma(R, distr_param.g_form, distr_param.g_mult);
				break;
		}
	}
	return i;
}
/*
int main(){
	double *rand = (double*)malloc(1000*sizeof(double));
	struct distr_param_s dp;
	dp.mean = 2;
	dp.g_form = 2;
	dp.g_mult = dp.mean/dp.g_form;

	dp.type = erlang;
	rand_vec(rand, 1000, dp);
	int i=0;
	while(i<1000) {
		printf("%f\n", rand[i++]);
	}
	return 0;
}
*/
