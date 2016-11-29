#include <algo2.h>


unsigned long long int cpu2(Context *cont){
    int i,j;
    unsigned long long y_min, surface_max = 0; 
    for (i=0;i<cont->nb_points-1;++i)
        for (j=i+1;j<cont->nb_points;++j)
        {
           if (j==i+1)
              y_min = cont->h;
           else
              y_min = MIN(y_min, cont->Points[j-1][1]);
           surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
         }  
    return surface_max;
}

__global__ void kernel_par_2(Context* cont){
    __shared__ int s_max[NB_THREADS];  // tableau contenant le surface_max de chaque thread
    int index = threadIdx.x + blockIdx.x * blockDim.x ;
    int nb_iter = 1; // cont->nb_points / NB_THREADS;
    int pos = nb_iter * index + (MIN(cont->nb_points%NB_THREADS, index));
    if (cont->nb_points % NB_THREADS >= threadIdx.x) //Cas ou le nombre de points n'est pas un multiple de notre NB_THREADS
        nb_iter++;
    //CALCUL
    int i, j, y_min;
    unsigned long long surface_max = 0;
    int cpt = 0;

  //  printf("Lancement du thread no %d\n", threadIdx.x);

    for (i=0;i<nb_iter;++i){
        for (j=pos+1;j<cont->nb_points;++j){
           cpt++;
           if (j==pos+1)
              y_min = cont->h;
           else 
              y_min = MIN(y_min, cont->Points[j-1][1]);
           surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[pos][0]));
        }
        pos++;
    }

  //  printf("Thread no %d finished \n", threadIdx.x);

  //  printf("thread no : %d surface_max = %llu, nb_iter = %d, pos = %d, nb_tour = %d\n",threadIdx.x, surface_max, nb_iter, pos, cpt);
    // stockage des resultats dans la mémoire partagée
    s_max[threadIdx.x] = surface_max;
    __syncthreads();

    /* Comparaison des resultats afin de retrouver la valeur maximale */
    int a = NB_THREADS/2;
    while(a>0 && a>threadIdx.x){
        unsigned long long i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
      //  printf("valeur de i = %d\n",i);
        __syncthreads();
        s_max[threadIdx.x] = i;
        __syncthreads();
        a = a >> 1;
    }
    __syncthreads();
    //cont->surface_max = s_max[0];

    if(threadIdx.x == 0)
        atomicMax(&(cont->surface_max), (unsigned long long int)s_max[0]);

}

__host__ unsigned long long int gpu2(Context* cont){

    unsigned long long surface_max = 0;
    Context * d_cont;

    printf("sending to gpu... ");
    d_cont = d_load(cont);
    printf(" sent !\n");

    printf("lancement du kernel : \n");
    cont->start = my_gettimeofday();
    kernel_par_2<<<cont->nb_points/NB_THREADS,NB_THREADS>>>(d_cont);
    printf("sortie du kernel\n");

    cudaMemcpy(&surface_max, &(d_cont->surface_max), sizeof(unsigned long long), cudaMemcpyDeviceToHost); // récupération du résultat
    cont->end = my_gettimeofday();

    d_free<<<1,1>>>(d_cont);

    return surface_max;

}



int algo2(Context *cont, int env)
{
    int surface_max = 0;

    switch (env){
        case CPU:
                cont->start = my_gettimeofday();
		surface_max = cpu2(cont);
                cont->end = my_gettimeofday();
		break;
        case GPU:
		printf("lancement du GPU\n");
                surface_max = gpu2(cont);
                break;
    }
  
    return surface_max;
}
