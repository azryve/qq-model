#include <stdlib.h>
#include <math.h>
#include "vecs.h"

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
