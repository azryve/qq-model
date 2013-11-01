#include <stdlib.h>
#include <stdio.h>

#define NO_QUERIES_RET 100
#define AVE_QTIMING 10
#define AVE_STIMING 10
#define TIMING_GEN_ERR 101

int get_timings(int *vector, const int vector_size, const int ave);
int get_timing(const int ave);
void getstats();

int avq = 10;
int avs = 8;

int clock;
int free_atime; 
int qatime;
	
double util = 0;
double ave_q = 0;
int qline = 0;

void getstats(){
	if(free_atime != -1){ util++; }
	ave_q+=qline;
}

int get_timings(int *vector, const int vector_size, const int ave) {
	int i = 0;
	while(i<vector_size) { 
		vector[i++] = get_timing(ave);
	}

	return 0;
}

int get_timing(const int ave){
	return ave; //temp determine
}

int main(int argc, char *argv[]){
	if(argc < 4) { return 1; }

	int query_count = strtol(argv[1], 0, 10);
	avq = strtol(argv[2], 0, 10);
	avs = strtol(argv[3], 0, 10);

	if(! query_count > 0) { return NO_QUERIES_RET; }
	int query_timings_size = query_count - 1;
	int *query_timings = (int*) malloc(query_timings_size*sizeof(query_timings[0]));	

	if(get_timings(query_timings, query_timings_size, avq)) {
		return TIMING_GEN_ERR;
	}

	clock = 0;         //absolute clock
	free_atime = -1; //at start its free
	qatime = 0;      //absolute time of newquery. first comes at 0  
	qline = 0;

	int i=0;         //indexer by timings vector
	do{
		if(free_atime == clock){ free_atime = -1; }
		if(qatime == clock && i<query_count) {
			qline++;
			qatime = clock+query_timings[i++];
		}
		if(free_atime == -1 && qline > 0){
			qline--;
			free_atime = clock + get_timing(avs);
		}
	
		clock++; //tic
		getstats();	
	} while(i<query_count || qline>0);

	util /= clock;
	ave_q /= clock;

	printf("Util: %f\n", util);
	printf("Av. queue size: %f\n", ave_q);

	return 0;
}
