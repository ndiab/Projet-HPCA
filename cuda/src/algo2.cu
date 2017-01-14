#include <algo2.h>

__global__ void kernel_par_2(Context* cont){
    __shared__ int s_max[NB_THREADS];  // tableau contenant le surface_max de chaque thread
    int i = threadIdx.x + blockIdx.x * blockDim.x ;
    int y_min, j;
    //CALCUL
    unsigned long long surface_max = 0;


     for (j=i+1;j<cont->nb_points;++j){
        if (j==i+1)
           y_min = cont->h;
        else 
           y_min = MIN(y_min, cont->Points[j-1][1]);
        surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
     }

    s_max[threadIdx.x] = surface_max;
    __syncthreads();

    /* Comparaison des resultats afin de retrouver la valeur maximale */
    int a = NB_THREADS >>1 ;
    while(a>0 && a>threadIdx.x){
        unsigned long long i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
        __syncthreads();
        s_max[threadIdx.x] = i;
        __syncthreads();
        a = a >> 1;
    }
    __syncthreads();

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
    kernel_par_2<<<cont->nb_points/NB_THREADS + 1,NB_THREADS>>>(d_cont);
    printf("sortie du kernel\n");

    cudaMemcpy(&surface_max, &(d_cont->surface_max), sizeof(unsigned long long), cudaMemcpyDeviceToHost); // récupération du résultat
    cont->end = my_gettimeofday();

    d_free<<<1,1>>>(d_cont);

    return surface_max;

}



int algo2(Context *cont, int env)
{
    int surface_max = 0;

    printf("lancement du GPU\n");
    surface_max = gpu2(cont);
    
    return surface_max;
}
