#include <algo1.h>
#include <omp.h>


unsigned long long int algo1(Context *cont)
{  
    cont->start = my_gettimeofday(); 

    int i,j,k;
    unsigned long long int y_min = (unsigned long long int)cont->h, surface_max = 0;

    for (i=0;i<cont->nb_points-1;++i)
	#pragma omp parallel for private(j,k) lastprivate(y_min)      
        for (j=i+1;j<cont->nb_points;++j)
        {
           if (j==i+1)
              y_min = cont->h;
           else if ( j-1 == i+1) // 1 seul point separe xi et xj (i.e: xi < pt <xj)
              y_min = cont->Points[i+1][1];
           else // au moins deux points separe xi et xj (i.e: xi< p1<p2 <xj)
           { 
              y_min = cont->Points[i+1][1];
              for (k=i+1;k<=j-1;++k)
                  y_min = MIN(y_min, cont->Points[k][1]);
        }
        surface_max = MAX(surface_max,y_min*(cont->Points[j][0] - cont->Points[i][0]));
    }
    cont->end = my_gettimeofday();
  
    return surface_max;
}
