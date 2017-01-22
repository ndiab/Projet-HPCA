#include <dvpr.h>
#include <omp.h>
#include <mpi.h>


unsigned long long int dvpr(int debut, int fin, Context* cont, int deep){
	/**
	*	Algorithme Diviser pour regner sur CPU
	**/
	int i, m;
	unsigned long long int a, b, c, y_min;


	/* Condition d'arret */
	if(fin - debut <2){
		return (cont->Points[debut][1]);
	}

	y_min = cont->Points[debut+1][1];
	m = debut+1;

	for(i = debut+1; i < fin; i++){
		if (cont->Points[i][1] < y_min){
			y_min = cont->Points[i][1];
			m = i;
		}
	}
	
	deep++;
	
	/* Condition pour ne pas faire de stack overflow */
	/*if(deep > 101000){
		overflow = TRUE ;
		//printf("deep = %d\n",deep);
		return 0;  
	}*/

	unsigned long long int sous_max, _max = 0;
	
	

	#pragma omp task shared(a) firstprivate(debut,m,deep) if(deep<12)
	a = dvpr(debut, m, cont, deep);
	#pragma omp task shared(b) firstprivate(debut,m,deep) if(deep<12)
	b = dvpr(m, fin, cont, deep);
	#pragma omp taskwait
	
	c = y_min * (cont->Points[fin][0] - cont->Points[debut][0]);

	
	sous_max = MAX(a,b);
	_max = MAX(sous_max, c);

	return _max;
}

unsigned long long int dvpr_mpi(Context* cont){
	int my_rank, m = 0, i, provided;
	MPI_Status status;
	unsigned long long int y_min = cont->h, a = 0, b = 0, c, sous_max, _max;
	MPI_Request req;

	//MPI_Init(NULL,NULL);
	MPI_Init_thread( 0, 0, MPI_THREAD_SERIALIZED, &provided );
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	/* 1 -- Chargement du fichier (chaque processus charge son fichier) */
	if (load_file(cont)){
		printf("Problème de chargement du fichier");
		return 1;
    	}
	
	if(my_rank == 0) cont->start = my_gettimeofday();

	/* 2 - Calcul du premier découpage */
	for(i = 1; i < cont->nb_points-1; i++){
		if (cont->Points[i][1] < y_min){
			y_min = cont->Points[i][1];
			m = i;
		}
	}
	
	/* 3 - Lancement de l'algorithme recursif en parallele */
	if(my_rank == 0){
		#pragma omp parallel
		{
		#pragma omp single
		a = dvpr(0, m, cont, 1);
		}
		MPI_Recv(&b, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD, &status);
	}else{
		#pragma omp parallel
		{
		#pragma omp single
		a = dvpr(m, cont->nb_points-1, cont, 1);
		}
		MPI_Isend(&a, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &req);
	}

	/* Fin de la region parallele MPI */ 
	MPI_Finalize();

	c = y_min * (cont->Points[cont->nb_points-1][0] - cont->Points[0][0]);

	
	sous_max = MAX(a,b);
	_max = MAX(sous_max, c);
	cont->end = my_gettimeofday();

	if(my_rank == 0){
        printf("Surface maximale = %llu\n", _max);

    
        printf("Temps total de calcul : %g sec\n", cont->end - cont->start);
    }

	return _max;
}
