#include <algo1.h>


__device__ void d_max(int* s_max){
    int i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
    __syncthreads();
    s_max[threadIdx.x] = i;
}


__global__ void kernel_1_dim1(Context* cont){
    __shared__ unsigned long long int s_max[NB_THREADS];  // tableau contenant le surface_max de chaque thread
    int i = threadIdx.x + blockIdx.x * blockDim.x ;


    //CALCUL
    int j, y_min;
    unsigned long long int surface_max = 0;

    if(i >= cont-> nb_points)
	s_max[threadIdx.x] = 0;
    else{
	    for (j=i+1;j<cont->nb_points;++j){
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
  
	    // stockage des resultats dans la mémoire partagée
	    s_max[threadIdx.x] = surface_max;
    }
    __syncthreads();

    /* Comparaison des resultats afin de retrouver la valeur maximale */
    int a = NB_THREADS>>1;
    while(a>0 && a>threadIdx.x){
        unsigned long long int i = MAX(s_max[2*threadIdx.x], s_max[2*threadIdx.x+1]);
        __syncthreads();
        s_max[threadIdx.x] = i;
        __syncthreads();
        a = a >> 1;
    }
    __syncthreads();

    if(threadIdx.x == 0)
        atomicMax(&(cont->surface_max), (unsigned long long int)s_max[0]);

}


__global__ void kernel_1_dim2(Context* cont){
    //int nb_threads = blockDim.x * blockDim.y;
    __shared__ unsigned long long int s_max[NB_THREADS];  // tableau contenant le surface_max de chaque thread du bloc courant
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;

    //CALCUL
    int y_min;
    int pos_shared_tab = threadIdx.x * blockDim.x + threadIdx.y;

    if (j <= i || j>= cont-> nb_points || i >= cont->nb_points){
        s_max[threadIdx.x] =  0;
    }else{

	    if (j==i+1)
	       y_min = cont->h;
	    else if ( j-1 == i+1) // 1 seul point separe xi et xj (i.e: xi < pt <xj)
	       y_min = cont->Points[i+1][1];
	    else if (j > i) // au moins deux points separe xi et xj (i.e: xi< p1<p2 <xj)
	    { 
	       y_min = cont->Points[i+1][1];
	       int k;
	       for (k=i+1;k<=j-1;++k)
		  y_min = MIN(y_min, cont->Points[k][1]);
	     }
	    s_max[pos_shared_tab] =  y_min*(cont->Points[j][0] - cont->Points[i][0]);
    }
    __syncthreads();

    /* Comparaison des resultats afin de retrouver la valeur maximale */
    int a = NB_THREADS >> 1;
    while(a>0 && a> pos_shared_tab){
	unsigned long long int i = MAX(s_max[2*pos_shared_tab], s_max[2*pos_shared_tab+1]);
	__syncthreads();
	s_max[pos_shared_tab] = i;
	__syncthreads();
	a = a >> 1;
    }

    __syncthreads();

   if(threadIdx.x == 0 && threadIdx.y == 0  && s_max[0] != 0)
        atomicMax(&(cont->surface_max), (unsigned long long int)s_max[0]);

}

__host__ unsigned long long int d_algo1(Context* cont){

    unsigned long long int surface_max = 0;
    Context * d_cont;
 
    printf("sending to gpu... ");
    d_cont = d_load(cont);
    printf(" sent !\n");


    dim3 threadsPerBlock(8,8);
    dim3 nbBlocks(cont->nb_points/threadsPerBlock.x + 1, cont->nb_points/threadsPerBlock.y + 1);
    
    printf("lancement du kernel ... \n");
    cont->start = my_gettimeofday();
    kernel_1_dim2<<<nbBlocks, threadsPerBlock>>>(d_cont);


    cudaMemcpy(&surface_max, &(d_cont->surface_max), sizeof(unsigned long long int), cudaMemcpyDeviceToHost); // récupération du résultat
    cont->end = my_gettimeofday(); 

    d_free<<<1,1>>>(d_cont);
    
    return surface_max;

}




unsigned long long int algo1(Context *cont, int env)
{  
    int surface_max = 0;

    printf("lancement du GPU\n");
    surface_max = d_algo1(cont);

    return surface_max;
}
