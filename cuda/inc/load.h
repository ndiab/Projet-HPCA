#ifndef LOAD_H
#define LOAD_H


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <struct.h>

int load_file(Context* cont);
Context* d_load(Context * cont);
__global__ void d_free(Context* d_cont);
void h_free(Context* cont);


#endif /* load.h */
