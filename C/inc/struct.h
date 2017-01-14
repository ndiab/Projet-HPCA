#ifndef STRUCT_H
#define STRUCT_H

#include <sys/time.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define CPU 0
#define GPU 1

#define NB_THREADS 64

typedef int bool;
#define TRUE  1
#define FALSE 0

extern double my_gettimeofday();

typedef struct _Context
{
	char* file_name;
	int **Points;
	unsigned int nb_points;
	unsigned int l;
	unsigned int h;
        unsigned long long int surface_max;
	double start;  /* Compute time */
	double end;
}Context;

extern bool overflow;


#endif /* struct.h */
