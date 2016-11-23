#include <Rectangle_Maximal.h>

/*
__global__ void kernel1(unsigned int *Points,unsigned int size)
{
unsigned int i = threadIdx.x
unsigned int j = threaddy.y
}
*/



int main(int argc, char* argv[])
{

    /* 1 - Initialisation du contexte */  
    Context *cont = (Context*) malloc(sizeof(Context));
    cont->file_name = argv[1];
    
    if (load_file(cont)){
	printf("Problème de chargement du fichier");
	return 1;
    }
	

    /* 2 - Lancement de l'algorithme demandé pour résoudre le problème */ 
    int Surface_max = 0;
    int env = atoi(argv[3]);

    switch (atoi(argv[2]))
    {
	case 1 :
	    Surface_max = algo1(cont, env);
	    break;
	case 2 :
	    Surface_max = algo2(cont, env);
	    break;
    }


    /* 3 - Affichage du resultat */
    printf("Surface maximale = %d\n", Surface_max);
    
    // deploiement sur GPU
    //dim3 threabParBlock(,,1)
    //dim3 tailleGrille (,,1)

    /* 4 - Libération de la mémoire */
    h_free(cont);

    return 0;
}
