#include <dvpr.h>

unsigned long long int dvpr_seq(int debut, int fin, Context* cont){
	/**
	*	Algorithme Diviser pour regner sur CPU
	**/
	int i, m, y_min = cont->Points[debut][1], b_debut = debut, b_fin = fin;
	unsigned long long int a, b, c;

	while(cont->Points[debut][0] == cont->Points[b_debut][0]){
		if (b_debut == b_fin)
			break;
		else
			b_debut++;
	}
	while(cont->Points[fin][0] == cont->Points[b_fin][0]){
		if (b_debut == b_fin)
			break;
		else
			b_fin--;
	}
			

	/* Condition d'arret */
	if(b_debut == b_fin){
		return (cont->Points[b_debut][1]);
	}


	for(i = b_debut+1; i < b_fin; i++){
		if (cont->Points[i][1] < y_min){
			y_min = cont->Points[i][1];
			m = i;
		}
	}

	a = dvpr_seq(debut, m, cont);
	b = dvpr_seq(m, fin, cont);
	c = y_min * (cont->Points[fin][0] - cont->Points[debut][0]);

	unsigned long long int sous_max = MAX(a,b);
	
	return MAX(sous_max, c);
}


unsigned long long int dvpr(Context* cont, int env){
	int surface_max = 0;

	switch (env){
		case CPU :
			dvpr_seq(0, cont->nb_points-1, cont);
	}

	return surface_max;
}
