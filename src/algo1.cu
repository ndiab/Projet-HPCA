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

__global__ void kernel_seq_1(Context *cont){
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


__device__ void d_max(int* s_max){
    int i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
    __syncthreads();
    s_max[threadIdx.x] = i;
}


__global__ void kernel_par_1(Context* cont){
    __shared__ int s_max[NB_THREADS];  // tableau contenant le surface_max de chaque thread
    int nb_iter = cont->nb_points / NB_THREADS;
    int pos = nb_iter * threadIdx.x + (MIN(cont->nb_points%NB_THREADS, threadIdx.x));
    if (cont->nb_points % NB_THREADS >= threadIdx.x) //Cas ou le nombre de points n'est pas un multiple de notre NB_THREADS
        nb_iter++;
    //CALCUL
    int i, j, y_min;
    int surface_max = 0;
    int cpt = 0;

    for (i=0;i<nb_iter;++i){
        for (j=pos+1;j<cont->nb_points;++j){
           cpt++;
           if (j==pos+1)
              y_min = cont->h;
           else if ( j-1 == pos+1) // 1 seul point separe xi et xj (i.e: xi < pt <xj)
              y_min = cont->Points[pos+1][1];
           else // au moins deux points separe xi et xj (i.e: xi< p1<p2 <xj)
           { 
              y_min = cont->Points[pos+1][1];
	      int k;
              for (k=pos+1;k<=j-1;++k)
                  y_min = MIN(y_min, cont->Points[k][1]);
           }
           surface_max = MAX(cont->surface_max,y_min*(cont->Points[j][0] - cont->Points[pos][0]));
        } 
    }

   // printf("thread no : %d surface_max = %d, nb_iter = %d, pos = %d, nb_tour = %d\n",threadIdx.x, surface_max, nb_iter, pos, cpt);
    // stockage des resultats dans la mémoire partagée
    s_max[threadIdx.x] = surface_max;
    __syncthreads();

    /* Comparaison des resultats afin de retrouver la valeur maximale */
    int a = NB_THREADS/2;
    while(a>0 && a>threadIdx.x){
        int i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
      //  printf("valeur de i = %d\n",i);
        __syncthreads();
        s_max[threadIdx.x] = i;
        __syncthreads();
        a = a >> 1;
    }
    __syncthreads();
    cont->surface_max = s_max[0];

}

__host__ int gpu1(Context* cont){

    int surface_max = 0;
    Context * d_cont;

    d_cont = d_load(cont);

    kernel_par_1<<<1,NB_THREADS>>>(d_cont); //premiers tests en sequentiel sur GPU   
     
    cudaMemcpy(&surface_max, &(d_cont->surface_max), sizeof(int), cudaMemcpyDeviceToHost); // récupération du résultat

    d_free<<<1,1>>>(d_cont);
    
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
