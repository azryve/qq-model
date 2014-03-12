#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "gsl/gsl.h"

#include <getopt.h>

#include "vecs.h"

#define MAX_OUTFILE 1024

size_t size = 10000;
timings_t av = 1;
double util = 0.5;
size_t colns=128;
int outfile = 0;

void print_usage(const char *filename){
	printf("%s [-f] -a <av> -u <util> -s <size>\n", filename);
}

size_t parse_model_param(const char *param_string, struct distr_param_s* params, size_t size) {
	const char *delim = "/";
	char *str;
	char **curs = &str;
	char *m;
	size_t i;

	str = (char*) calloc(1024,sizeof(char));
	strncpy(str, param_string, 1024-1);

	i=0;
	while((m = strsep(curs, delim)) && (i < size)) {
		struct distr_param_s *p = &params[i];
		switch(m[0]){
			case 'M':
				p->type = exponential;
				break;
			case 'E':
				p->type = erlang;
				p->g_form = 0;

				p->g_form = strtod(&m[1], 0);
	
				if (p->g_form <= 0 || ERANGE == errno) {
                    p->g_form = 1;
                    printf("Bad form for E provided, set to %f\n", p->g_form);
                }
				break;
			
			default:
				continue;
		}
		i++;
	}

	return i; 

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
	struct distr_param_s dp_v[2];				

	char opt;
	while((opt=getopt(argc, argv, "u:a:s:fc:m:")) != -1){
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
			case('m'):
				if( 2 != parse_model_param(optarg, dp_v, 2)) {
					printf("Model : parse fail\n");
					break;
				}
				int z;
				for(z = 0; z < 2; z++){ 
					if (dp_v[z].type == exponential) { 
						printf("Model : exp\n"); 
					} else { 
						printf("Model : erlang\n"); 
					}
				}
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


	dp_v[0].mean = av;
	dp_v[1].mean = dp_v[0].mean * util;
	
	dp_v[0].g_mult = dp_v[0].mean / dp_v[0].g_form;
	dp_v[1].g_mult = dp_v[1].mean / dp_v[1].g_form;
	

	rand_v((double*) q_vec, size, dp_v[0]);	/*generating query intervals*/

	rand_v((double*) s_vec, size, dp_v[1]);	/*generatig serving intervals*/
	
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

		size_t s = size;
		double *output_vec = (double*) malloc(s * sizeof(double));
		output_vec[0] = q_vec[0];
		double aq = 0;
		for(i=1; i < s; ++i) {
			aq+=q_vec[i];
			output_vec[i] = fmax(aq, output_vec[i-1]) + s_vec[i]; 			/* if i-1 query left before i arrived take aq (absolute clock
																			else take i-1 departue time + i serve time */
		}
		for(i=s - 2; i > 0; --i) {
			output_vec[i] -= output_vec[i-1];
		}
		out_vec("./latex/data.depart", output_vec, colns);
		free(output_vec);
	}

	end:
		return rc;
}
