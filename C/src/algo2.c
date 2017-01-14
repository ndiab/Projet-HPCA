#include <algo2.h>


unsigned long long int algo2(Context *cont){
    int i,j;
    unsigned long long y_min = cont->h , surface_max = 0; 
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
