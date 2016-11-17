#include <load.h>


int load_file(Context* cont)
{
    //int **Points;
    FILE *fichier = NULL;
    char *chaine = NULL;
    size_t longueur = 0;
    int i = 0, j = 0;
    fichier = fopen(cont->file_name,"r");
    if (fichier != NULL)
    { 
        int compteur = 1;
        while ((getline(&chaine, &longueur, fichier)) != -1)  //chargement des points donnés dans le fichier d'entrée
        { //Attention a la taille limite d'un fichier sur le systeme  
            sscanf(chaine,"%d %d",&i,&j);
            if (compteur==1) // taille du repère
               cont->l = i, cont->h = j;
            else if (compteur == 2) // nombre de points
            {  cont->nb_points = i;
               cont->Points = (int**)malloc(cont->nb_points*sizeof(int *));
	       int k;
               for (k=0; k< cont->nb_points; ++k)
               {
                  cont->Points[k] =(int*) malloc(2*sizeof(int));
                  cont->Points[k][0] = i;
                  cont->Points[k][1] = j;
               }
            }
            else if (compteur-3 < cont->nb_points)
            { 
                  cont->Points[compteur - 3][0] = i;
                  cont->Points[compteur - 3][1] = j;
            }
            compteur += 1;
        }
        fclose(fichier);
    }
    else
       return 1; 

    free(chaine);

    return 0;
	
}

__host__ Context* send_gpu(Context *cont){
    /*
    Alloue la mémoire et copie les données du context CPU -> GPU.
    retourne le pointeur sur le contexte du device
    */

    int k;
    Context *cont_gpu; //device copy of cont
    printf("test 3");
    /* Memory device allocation */
    cudaMalloc(&cont_gpu, sizeof(Context));
    /* Memory allocation of Points tab */
    cudaMalloc(cont_gpu->Points , cont->nb_points*sizeof(int *));
    for (k=0; k< cont_gpu->nb_points; ++k)
    {
        cudaMalloc((void**) cont_gpu->Points[k], 2*sizeof(int));
    }

    printf("test 4");
    /* Copie des données sur GPU */
    cudaMemcpy(cont_gpu,cont,sizeof(Context),cudaMemcpyHostToDevice);
    cudaMemcpy(cont_gpu->Points, cont->Points, cont->nb_points*sizeof(int *),cudaMemcpyHostToDevice);
    for (k=0; k< cont_gpu->nb_points; ++k)
    {
        cudaMemcpy(cont_gpu->Points[k], cont->Points[k], 2*sizeof(int) ,cudaMemcpyHostToDevice);
    }

    printf("test 5");

    return cont_gpu;
}


void dealloc(Context* cont){
    int k;
    for (k=0; k<cont->nb_points; ++k)
        free(cont->Points[k]);
    free(cont->Points);
    free(cont);
}
