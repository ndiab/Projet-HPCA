#include <algo2.h>


int sequentiel2(Context *cont){
    int i,j, y_min = 0, surface_max = 0;
    for (i=0;i<cont->nb_points-1;++i)
        for (j=i+1;j<cont->nb_points;++j)
        {
           if (j==i+1)
              y_min = cont->h;
           else
              y_min = MIN(y_min, cont->Points[j-1][1]);
           surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
         }  
    return surface_max;
}

int algo2(Context *cont, int env)
{
    int surface_max = 0;

    switch (env){
        case CPU:
		surface_max = sequentiel2(cont);
		break;
        case GPU:
		break;
    }
  
    return surface_max;
}
