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

