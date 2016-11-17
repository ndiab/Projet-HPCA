#ifndef LOAD_H
#define LOAD_H


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <struct.h>


extern int load_file(Context* cont);
extern Context* send_gpu(Context * cont);
extern void dealloc(Context* cont);


#endif /* load.h */
