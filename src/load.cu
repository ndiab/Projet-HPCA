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

__host__ Context* d_load(Context *h_cont){
    /*
    Alloue la mémoire et copie les données du context CPU -> GPU.
    retourne le pointeur sur le contexte du device
    */

    Context *d_cont, *l_cont; //device copy of cont
    l_cont = (Context*)malloc(sizeof(Context));
    int **d_points, **l_points, k;
    l_points = (int**)malloc(h_cont->nb_points*sizeof(int *));

    for(k = 0; k < h_cont->nb_points; k++){
        int* point;
        cudaMalloc((void**) &point, 2*sizeof(int));
        cudaMemcpy(point, h_cont->Points[k], 2*sizeof(int), cudaMemcpyHostToDevice);
        l_points[k] = point;
    }

    cudaMalloc((void**) &d_points, h_cont->nb_points*sizeof(int *));
    cudaMemcpy(d_points, l_points, h_cont->nb_points*sizeof(int *), cudaMemcpyHostToDevice);
    memcpy(l_cont, h_cont, sizeof(Context));
    l_cont->Points = d_points;

    cudaMalloc((void**) &d_cont, sizeof(Context));
    cudaMemcpy(d_cont, l_cont, sizeof(Context), cudaMemcpyHostToDevice);
    

    return d_cont;
}

__global__ void d_free(Context* d_cont){
    int k;
    for (k=0; k< d_cont->nb_points; ++k)
       free(d_cont->Points[0]);
    free(d_cont->Points);
    free(d_cont);
}


void h_free(Context* cont){
    int k;
    for (k=0; k<cont->nb_points; ++k)
        free(cont->Points[k]);
    free(cont->Points);
    free(cont);
}
