typedef  double timings_t;
typedef timings_t* timev_t;

int calculate_wait_vec(timev_t qt, timev_t st, timev_t wt,size_t size);

double ave_vec(double *vec, size_t size);
