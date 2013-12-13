all:
	gcc -o model model.c

rand:
	gcc -o rand_test rand_test.c

new:
	gcc -o model_new model_new.c gsl/gsl.c -I /opt/local/include -Wl,/opt/local/lib/libgsl.a
