all: model

model:
	gcc  -O3 -o model model.c vecs.c gsl/gsl.c -lgsl -lgslcblas -lm


pdfclean:
	make -C latex clean

pdf: pdfclean
	make -C latex all
	
