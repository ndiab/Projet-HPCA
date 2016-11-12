#ifndef STRUCT_H
#define STRUCT_H


typedef struct _Context
{
	char* file_name;
	int **Points;
	unsigned int nb_points;
	unsigned int l;
	unsigned int h;
}Context;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


#endif /* struct.h */
