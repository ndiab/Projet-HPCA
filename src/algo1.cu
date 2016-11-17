#include <algo1.h>

int cpu1(Context *cont){

    int i,j, y_min, surface_max = 0;
    for (i=0;i<cont->nb_points-1;++i)
        for (j=i+1;j<cont->nb_points;++j)
        {
           if (j==i+1)
              y_min = cont->h;
           else if ( j-1 == i+1) // 1 seul point separe xi et xj (i.e: xi < pt <xj)
              y_min = cont->Points[i+1][1];
           else // au moins deux points separe xi et xj (i.e: xi< p1<p2 <xj)
           { 
              y_min = cont->Points[i+1][1];
	      int k;
              for (k=i+1;k<=j-1;++k)
                  y_min = MIN(y_min, cont->Points[k][1]);
           }
           surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
        }

    return surface_max;

}

__global__ void kernel1(Context *cont){
    //int index = threadIdx.x + blockIdx.x * blockDim.x;

    int i, j, y_min;
    cont->surface_max = 0;
    for (i=0;i<cont->nb_points-1;++i)
        for (j=i+1;j<cont->nb_points;++j)
        {
           if (j==i+1)
              y_min = cont->h;
           else if ( j-1 == i+1) // 1 seul point separe xi et xj (i.e: xi < pt <xj)
              y_min = cont->Points[i+1][1];
           else // au moins deux points separe xi et xj (i.e: xi< p1<p2 <xj)
           { 
              y_min = cont->Points[i+1][1];
	      int k;
              for (k=i+1;k<=j-1;++k)
                  y_min = MIN(y_min, cont->Points[k][1]);
           }
           cont->surface_max = MAX(cont->surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
        }
}

__host__ int gpu1(Context* cont){

    int surface_max = 0;
    Context * d_cont;

    d_cont = send_gpu(cont);

    kernel1<<<1,1>>>(d_cont); //premiers tests en sequentiel sur GPU   
     
    cudaMemcpy(&surface_max, &(d_cont->surface_max), sizeof(int), cudaMemcpyDeviceToHost);
    
    return surface_max;

}




int algo1(Context *cont, int env)
{  
    int surface_max = 0;

    switch (env){
        case CPU:
		surface_max = cpu1(cont);
		break;
        case GPU:
		surface_max = gpu1(cont);
		break;
    }
  
    return surface_max;
}
