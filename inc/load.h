#ifndef LOAD_H
#define LOAD_H


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <struct.h>

extern int load_file(Context* cont);
extern Context* d_load(Context * cont);
extern __global__ void d_free(Context* d_cont);
extern void h_free(Context* cont);


#endif /* load.h */
