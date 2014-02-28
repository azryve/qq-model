all:
	gcc -o model model.c

rand:
	gcc -o rand_test rand_test.c

new:
	gcc  -O3 -o model_new model_new.c vecs.c gsl/gsl.c -lgsl -lgslcblas -lm
