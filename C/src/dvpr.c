#include <dvpr.h>
#include <omp.h>


unsigned long long int dvpr(int debut, int fin, Context* cont, int deep){
	/**
	*	Algorithme Diviser pour regner sur CPU
	**/
	int i, m, y_min, b_debut = debut, b_fin = fin;
	unsigned long long int a, b, c;


	/* Condition d'arret */
	if(b_fin - b_debut <2){
		return (cont->Points[b_debut][1]);
	}

	y_min = cont->Points[b_debut+1][1];
	m = b_debut+1;

	for(i = b_debut+1; i < b_fin; i++){
		if (cont->Points[i][1] < y_min){
			y_min = cont->Points[i][1];
			m = i;
		}
	}

	deep++;
	
	/* Condition pour ne pas faire de stack overflow */
	if(deep > 101000){
		overflow = TRUE ;
		//printf("deep = %d\n",deep);
		return 0;  
	}

	unsigned long long int sous_max, _max;
	
	#pragma omp parallel
	{
	#pragma omp task shared(a)
	a = dvpr(debut, m, cont, deep);
	#pragma omp task shared(b)
	b = dvpr(m, fin, cont, deep);

	c = y_min * (cont->Points[fin][0] - cont->Points[debut][0]);

	#pragma omp taskwait
	sous_max = MAX(a,b);
	_max = MAX(sous_max, c);
	}

	return _max;
}
