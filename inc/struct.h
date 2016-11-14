#ifndef STRUCT_H
#define STRUCT_H

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct _Context
{
	char* file_name;
	int **Points;
	unsigned int nb_points;
	unsigned int l;
	unsigned int h;
}Context;


#endif /* struct.h */
