#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "gsl/gsl.h"

#include <getopt.h>

#include "vecs.h"

#define MAX_OUTFILE 1024

size_t size = 10000;
timings_t av = 1;
double util = 0.5;
size_t colns=128;
double erlang_form = 5;
int outfile = 0;

void print_usage(const char *filename){
	printf("%s [-f] -a <av> -u <util> -s <size>\n", filename);
}

void out_vec(const char *file, const double *print_vec, size_t size){
  	FILE *fo;
	size_t i;

	if(NULL == (fo = fopen(file, "w+"))){
        perror("Output error");
        return;
    }
	
	for(i=0; i < size; ++i) {
		fprintf(fo, "%zu\t\%f\n", i, print_vec[i]);
	}
	
	fclose(fo);	
}

void out_vec_bars(const char *file, const double *print_vec, size_t size,size_t cl){
	size_t *bars = (size_t*) calloc(sizeof(size_t),colns);
	double max = 0, min = 0;
	double delta = 0;	
	FILE *fo;

	if(NULL == (fo = fopen(file, "w+"))){
		perror("Output error");
		return;
	}

	size_t i,j;
	for(i = 0; i < size; ++i) {
		min=fmin(min, print_vec[i]);
		max=fmax(max, print_vec[i]);				
	}
	delta = (max-min)/colns;
	if(0 == delta) { 
		delta++;
	}	

	for(i = 0; i < size; ++i) {
		j = (print_vec[i]-min)/delta;
		bars[j]++;
	}
	
	for(i = 0; i < colns; ++i) {
		double norm = bars[i];
		norm /= size;
		fprintf(fo, "%f\t%f\n", (delta*(i+1))+min, norm);	
	}

	fclose(fo);
	free(bars);

}

int main(int argc, char* argv[]){
	int rc=0;
	char *q_file = 0;
	char *s_file = 0;

	char opt;
	while((opt=getopt(argc, argv, "u:a:s:fc:e:")) != -1){
		switch(opt){
			case('f'):
				outfile = 1;
				break;
			case('c'):
				colns=strtoul(optarg, 0, 10);		
				break;
			case('u'):
				util = (timings_t) strtod(optarg, 0);
				break;
			case('a'):
				av = (timings_t) strtod(optarg, 0);
				break;
			case('s'):
				size = (size_t) strtoul(optarg, 0, 10);
				break;
			case('e'):
				erlang_form = (double) strtod(optarg, 0);
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
	dp.type=erlang;
	dp.g_form=10;
	dp.mean=(double) av;
	dp.g_mult=dp.mean/dp.g_form;
	rand_v((double*) q_vec, size, dp);	/*generating query intervals*/

	dp.mean*=util;
	dp.type=erlang;
	dp.g_form=5;
	dp.g_mult=dp.mean/dp.g_form;
	rand_v((double*) s_vec, size, dp);	/*generatig serving intervals*/
	
	calculate_wait_vec(q_vec, s_vec, w_vec, size);
	
	printf("Среднее время интервалов запросов:\t%f\n", ave_vec(q_vec, size));
	printf("Cреднее время обслуживания запросов:\t%f\n", ave_vec(s_vec, size));
	printf("Среднее время ожидания запроса в очереди:\t:%f\n", ave_vec(w_vec, size));

	if(outfile) {
		out_vec_bars("./latex/data.serv", s_vec, size, colns);
		out_vec_bars("./latex/data.gen", q_vec, size, colns);
		out_vec("./latex/data.wait", w_vec + 1000, 25); // +1000 to let quque stabilize

		size_t i,j;
		double *vec = w_vec;
		double *filtered_vec = malloc(size*sizeof(double));
		size_t filtered_size;
		for(i=0, j=0; i < size; i++){
			if(vec[i] > 0) { filtered_vec[j++] = vec[i]; }
		}
		filtered_size = j+1;
		out_vec_bars("./latex/data.wait.bars", filtered_vec, filtered_size, colns);
		free(filtered_vec);
 
	}

	end:
		return rc;
}
