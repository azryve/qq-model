enum distr_type_e {
	exponential,
	erlang
};

struct distr_param_s {
	double mean;
	double sd;
	double g_form;
	double g_mult;
	enum distr_type_e type;
};


size_t rand_v(double *vec, size_t vec_size,  struct distr_param_s distr_param);
