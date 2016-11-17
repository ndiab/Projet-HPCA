#ifndef STRUCT_H
#define STRUCT_H

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define CPU 0
#define GPU 1

typedef struct _Context
{
	char* file_name;
	int **Points;
	unsigned int nb_points;
	unsigned int l;
	unsigned int h;
        int surface_max;
}Context;


#endif /* struct.h */
