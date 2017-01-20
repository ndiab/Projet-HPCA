#include <Rectangle_Maximal.h>

bool overflow = FALSE;

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}


int main(int argc, char* argv[])
{

    /* 1 - Initialisation du contexte */  
    Context *cont = (Context*) malloc(sizeof(Context));
    cont->file_name = argv[1];
    
    printf("loading...");
    if (load_file(cont) && atoi(argv[2])!=4){ // 4 représente la fonction lancée avec MPI qui a son propre chargement de fichier
	printf("Problème de chargement du fichier");
	return 1;
    }
    printf("loaded !\n");

    cont-> surface_max = 0;
	
    /* 2 - Lancement de l'algorithme demandé pour résoudre le problème */ 
    unsigned long long int Surface_max = 0;
    
    switch (atoi(argv[2]))
    {
	case 1 :
	    Surface_max = algo1(cont);
	    break;
	case 2 :
	    Surface_max = algo2(cont);
	    break;
	case 3 :
	    cont->start = my_gettimeofday(); 
	    #pragma omp parallel
	    {
	    #pragma omp single
	    Surface_max = dvpr(0, cont->nb_points-1, cont, 0);
	    }
	    cont->end = my_gettimeofday();
	    break;
	case 4 :
	    Surface_max = dvpr_mpi(cont);
    }


    /* 3 - Affichage du resultat */
    printf("Surface maximale = %llu\n", Surface_max);

    /* 4 - Libération de la mémoire */
    printf("Temps total de calcul : %g sec\n", cont->end - cont->start);
    h_free(cont);

    if(overflow){
	printf(
		"********** WARNING **********\nLA RECURSION EST BEAUCOUP TROP IMPORTANTE POUR LE SYSTEME\nPOUR EVITER UN DEBORDEMENT DE LA PILE LA RECURSION A ETE INTERROMPUE\nLE RESULTAT PEUT ETRE INCORECT\n"
	);
    }

    return 0;
}
