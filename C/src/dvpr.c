#include <dvpr.h>
#include <omp.h>


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
