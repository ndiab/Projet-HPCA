#include <load.h>


int load_file(Context* cont)
{
    //int **Points;
    FILE *fichier = NULL;
    char *chaine = NULL; 
    ssize_t read;
    size_t longueur = 0;
    int i = 0, j = 0;
    fichier = fopen(cont->file_name,"r");
    if (fichier != NULL)
    { 
        int compteur = 1;
        while ((read = getline(&chaine, &longueur, fichier)) != -1)  //chargement des points donnés dans le fichier d'entrée
        { //Attention a la taille limite d'un fichier sur le systeme  
            sscanf(chaine,"%d %d",&i,&j);
            if (compteur==1) // taille du repère
               cont->l = i, cont->h = j;
            else if (compteur == 2) // nombre de points
            {  cont->nb_points = i;
               cont->Points = malloc(cont->nb_points*sizeof(int *));
	       int k;
               for (k=0; k< cont->nb_points; ++k)
               {
                  cont->Points[k] = malloc(2*sizeof(int));
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


void dealloc(Context* cont){
    for (int k=0; k<cont->nb_points; ++k)
        free(cont->Points[k]);
    free(cont->Points);
    free(cont);
}
