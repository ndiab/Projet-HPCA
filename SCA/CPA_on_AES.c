#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#define NB_MESURES 100 /* Correspond au nombre de plaintext */
#define NB_POINTS 3000

/*
 * Forward S-box
 */
static const unsigned char FSb[256] =
{
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
    0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
    0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
    0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
    0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
    0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
    0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
    0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
    0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
    0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
    0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/* path to aes s-BOX : /root/chipwhisperer/hardware/victims/firmware/crypto/avrcryptolib/aes*/

int extraction_donnees(unsigned char **t_plaintexts, float **t_traces, char *path_fic){
  int i,k;
  FILE *fic_csv;

  /* Ouverture du fichier csv */
  if( (fic_csv = fopen(path_fic, "rb")) == NULL){
    printf("%d : %s\n", errno, strerror(errno));
    printf("Erreur lors de l'appel a fopen\n");
    return EXIT_FAILURE;
  }
  
  for (i=0;i<NB_MESURES;i++){
    
    /* Allocation du i-ieme plaintext **/
    /* Remarque : on lit 16octets mais chacun est codé sur 2 caracteres : 32 unsigned char */
    t_plaintexts[i] = (unsigned char *)malloc(sizeof(unsigned char)*32);
    if(t_plaintexts[i] == NULL){
      printf("Erreur lors du malloc de t_plaintexts[%d] dans extraction donnees\n",i);
      return EXIT_FAILURE;
    }

    /* Lecture du ieme plaintext */
    if( fread(t_plaintexts[i], sizeof(unsigned char), 32, fic_csv) != 32 ){
      printf("%d : %s\n", errno, strerror(errno));    
      printf("Erreur lors de l'appel a fread du plaintext\n");
      return EXIT_FAILURE;
    }

    /* Decalage pour lire la trace : on saute key (32caracteres), ', ' et ', ' soit 32+4  */
    fseek(fic_csv, 32 + 4, SEEK_CUR);

    /* Allocation de la i-ieme trace */
    t_traces[i] = (float *)malloc(sizeof(float)*NB_POINTS);
    if(t_traces[i] == NULL){
      printf("Erreur lors du malloc de t_traces[%d] dans extraction de donnes\n",i);
      return EXIT_FAILURE;
    }
    
    /* Lecture de la i-ieme trace */
    for(k=0;k<NB_POINTS-1 ;k++){
      if( fscanf(fic_csv, "%f, ",&(t_traces[i][k])) != 1 ){
	printf("%d : %s\n", errno, strerror(errno));    
	printf("Erreur lors de l'appel a fscanf de t_traces[%d][%d] \n",i,k);
	return EXIT_FAILURE;
      }
    }
    if( fscanf(fic_csv, "%f ",&(t_traces[i][k])) != 1 ){
      printf("%d : %s\n", errno, strerror(errno));    
      printf("Erreur lors de l'appel a fscanf de t_traces[%d][%d] \n",i,k);
      return EXIT_FAILURE;
    }
  }

  /* Fermeture du fichier */
  if(fclose(fic_csv) == EOF){
    printf("%d : %s\n", errno, strerror(errno));    
    printf("Erreur lors de l'appel a fclose\n");
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}

int ecriture_score(float **scores, char *path_fic2){
	int i, j;
	FILE* fic_out;
	fic_out =  fopen(path_fic2, "w");
	if(fic_out == NULL){
		printf("erreur ouverture de fichier out\n");
		return EXIT_FAILURE;
	}
	
	fprintf(fic_out, "------------- CE FICHIER CONTIENT LA MATRICE DES SCORES --------------\n");
  
	for(i=0; i<256; i++){
		for(j=0; j<NB_POINTS-1 ; j++){
			fprintf(fic_out, "%f ,", scores[i][j]);
		}
		fprintf(fic_out, "%f", scores[i][j]);
		fprintf(fic_out, "\n");
 	}

	/* Fermeture du fichier */
	if(fclose(fic_out) == EOF){
		printf("%d : %s\n", errno, strerror(errno));    
		printf("Erreur lors de l'appel a fclose\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*! \fn float f_correlationCoefficient ( float *X, float **Y, UINT v_sampleSize)
 \param X is a vector of size v_samplesize , Y is a matrix with v_sampleSize rows
 \brief process the correlation coefficient
 \brief corr(X,Y[j]) = (EXY - EX* EY)/(sqrt(EXX - EX*EX)*sqrt(EYY-EY*EY))
 \brief where Y[j] corresponds to the jth column of Y and
 \brief where EX (resp EY) denotes the mean and EXX (resp EYY) denotes the 2nd-order
  moment
 \return an array of correlation values ( float ) (one correlation value for each j in 
  [v_indexStart .. v_indexEnd]
*/
/**
Faire les calculs de correlation pour j allant de  v_indexStart a v_indexEnd correspond a tester
la correlation pour chaque point de la trace dont l'indice est compris entre ces 2 variables.
Les indices de lignes de Y correspondent, eux, aux differents plaintexts (il y en a v_sampleSize).
**/
float* f_correlationCoefficient(float *X, float **Y, unsigned int v_sampleSize, unsigned int
				 v_indexStart, unsigned int v_indexEnd){
  int i,j;
  float EX, EY_j, EXX, EY_jY_j, EXY_j, num, den;

  /* Allocation de correlation_value */
  float *correlation_value = (float *)malloc(sizeof(float)*(v_indexEnd - v_indexStart + 1));
  if( correlation_value == NULL ){
    printf("Erreur lors de l'appel a malloc dans f_correlationCoefficient\n");
    return NULL;
  }
  
  /* Calcul de correlation pour chaque point de la trace (correspond a chaque colonne de Y) */
  for(j=v_indexStart; j <= v_indexEnd; j++){
    /* Initialisation */    
    correlation_value[j] = -1.; 

    /* Calcul de EX, EY[j], EXX, EY[j]Y[j] et de EXY[j] */
    EX = 0.;
    EY_j = 0.;
    EXX = 0.;
    EY_jY_j = 0.;
    EXY_j = 0.;
    for(i=0;i<v_sampleSize;i++){
      EX += X[i];
      EY_j += Y[i][j];
      EXX += (X[i] * X[i]);
      EY_jY_j += (Y[i][j] * Y[i][j]);
      EXY_j += (Y[i][j] * X[i]);      
    }
    EX /= v_sampleSize;
    EY_j /= v_sampleSize;
    EXX /= v_sampleSize;
    EY_jY_j /= v_sampleSize;
    EXY_j /= v_sampleSize;

    /* Calcul du numerateur de la correlation */ 
    num = v_sampleSize * (EXY_j - EX * EY_j);

    /* Calcul du denominateur de la correlation */
    /* Rappel : E((x-E(x))^2) = E(x^2) - E(x)^2 */
    den = sqrtf(v_sampleSize * (EXX - EX*EX) * v_sampleSize * (EY_jY_j - EY_j*EY_j));

    if(den!=0.){
      correlation_value[j] = num/den;
    }else{
      correlation_value[j] = 0.;
    }

    /* DEBUG */
    /**
    if(j==1705){
      printf("X = ");
      for(i=0;i<v_sampleSize;i++){
	printf("%f, ", X[i]);
      }

      printf("\n");
      printf("Y[i][j] = ");
      for(i=0;i<v_sampleSize;i++){
	printf("%f, ", Y[i][j]);
      }
      printf("\n");
      printf("Correlation = %f\n", correlation_value[j]);
    }
    **/
  }
  return correlation_value;
}

/* Calcul de HW[v_keyHyp ^ t_plaintext[i]], c'est a dire du nombre de bit de difference */
/* On ne calcule le poids de Hamming que pour l' octet de poids fort de t_plaintext[i] car v_keyHyp ne fait qu'un octet */
float Poids_Hamming(unsigned int cle, unsigned char *plaintext){
  int eight_bits_key, eight_bits_plaintext;
  int k;
  int Res_sbox_hex, xor;
  float HW=0.;
 
    eight_bits_key = cle;

    /* On recupere les 8 bits de poids forts du plaintext et on les transforme de ascii vers entier*/
    if( plaintext[0] >= '0' && plaintext[0] <= '9'){
      eight_bits_plaintext = plaintext[0] - '0'; 
    }else{
      eight_bits_plaintext = plaintext[0] - 'a' + 10;
    }
    if( plaintext[1] >= '0' && plaintext[1] <= '9'){
      eight_bits_plaintext = eight_bits_plaintext * 16 + plaintext[1] - '0'; 
    }else{
      eight_bits_plaintext = eight_bits_plaintext * 16 + plaintext[1] - 'a' + 10;
    }	

    /* Calcul du xor de ces 2 octets */
    xor = eight_bits_key ^ eight_bits_plaintext;

    /* Calcul du resultat de la Sbox du xor */
    Res_sbox_hex = FSb[xor];

    /* On calcule le poids de Hamming de la valeur hexadecimale Sbox[hyp_key^plaintext]  */
    for(k=0;k<8;k++){
      HW += (float)(((Res_sbox_hex)>>k) & 1);
    }


  return HW;
}

/* 
Renvoie un tableau de score (de correlation) 
avec selon les lignes, les hypotheses de cle, 
selon les colonnes, les indices des points de la traces etudiees 
*/ 
float** Calcul_score(unsigned char **t_plaintexts, float **t_traces){
  float **score;
  float *t_predictions;
  unsigned int v_keyHyp;
  int i;

  /* Allocation du tableau des scores et de celui des predictions */
  score = (float **)malloc(sizeof(float*)*256);
  if(score == NULL){
    printf("Erreur lors de l'appel a malloc de score\n");
    return NULL;
  }
  t_predictions = (float *)malloc(sizeof(float)*NB_MESURES);
  if(t_predictions == NULL){
    printf("Erreur lors de l'appel a malloc de t_predictions\n");
    return NULL;
  }

  /* Calcul, pour chaque hypothese de cles et chaque instant de mesure, 
     des correlations entre le vecteur de prediction et le vecteur de mesure.
     Chaque composante de ces 2 vecteurs correspond a une mesure(= 1 plaintext) differente.  */
  for(v_keyHyp=0; v_keyHyp < 256; v_keyHyp++){
    
    /* Calcul des predictions selon le modele du poids de Hamming */
    for(i=0;i<NB_MESURES;i++){
      t_predictions[i] = Poids_Hamming(v_keyHyp, t_plaintexts[i]);
    }
    
    /* Calcul des correlations entre les predictions et les traces */    
    score[v_keyHyp] = f_correlationCoefficient(t_predictions, t_traces, NB_MESURES, 0, NB_POINTS-1);
    if(score[v_keyHyp] == NULL){
      printf("Erreur lors de l'appel a f_correlationCoefficient dans Calcul_score\n");
      return NULL;
    }   
  }
  
  free(t_predictions);
  return score;
}

void Calcul_max_2D(float **tab){
  int i,j, nb_nan=0;
  float max = -1;
  int i_ind=-1, j_ind=-1;
  
  for(j=0;j<NB_POINTS;j++){
    for(i=0;i<256;i++){

      if(!isnan(tab[i][j])){
	if(tab[i][j] < 0){

	  //DEBUG : affichage des meilleurs valeurs
	  /**
	  if(tab[i][j] < -0.34){
	    printf("tab[%d][%d] = %f\n",i,j,tab[i][j]);		   
	  }
	  **/
	     
	  if(max < -tab[i][j]){
	    max = -tab[i][j];
	    i_ind = i;
	    j_ind = j;
	  }
	}else{

	  //DEBUG : affichage des meilleurs valeurs
	  /**
	  if(tab[i][j] > 0.34){
	    printf("tab[%d][%d] = %f\n",i,j,tab[i][j]);		   
	  }
	  **/
	  
	  if(max < tab[i][j]){
	    max = tab[i][j];
	    i_ind = i;
	    j_ind = j;
	  }
	}
      }else{
	nb_nan++;
	printf("Attention : %d valeurs de score sont des nan dont score[%d][%d]\n",nb_nan,i,j);
      }
    }
  }
  
  /* DEBUG */
 /**
  for(j=0;j<NB_POINTS;j++){
    if(tab[i_ind][j] > 0.3 || tab[0][j] < -0.3){
      printf("tab[%d][%d] = %f\n",i_ind, j,tab[i_ind][j]);
    }
  }
  **/
  printf("max = %f =  tab[%d][%d] = %f\n",max, i_ind,j_ind, tab[i_ind][j_ind]);
  printf("Le 1er octet de cle trouve est %d ! \n", i_ind);
}

int main(int argc, char *argv[]){
  unsigned char **t_plaintexts;
  float **t_traces, **score;
  char *path_to_fic, *path_to_fic2; 
  int i;// ind_x, ind_y;
  
  if(argc != 3){
    printf("Usage : %s path_to_fic path_to_fic_sortie\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* Allocation */
  t_plaintexts=(unsigned char **)malloc(sizeof(unsigned char *)*NB_MESURES);  // tableau de chaine de caractères (plaintext)
  t_traces=(float **)malloc(sizeof(float *)*NB_MESURES); // 100 tableaux de 3000 mesures
  path_to_fic = (char *)malloc(sizeof(char) *100);  // chemin vers le fichier
  path_to_fic2 = (char *)malloc(sizeof(char) *100);  // chemin vers le fichier2
  if(t_plaintexts == NULL || t_traces == NULL || path_to_fic == NULL || path_to_fic2 == NULL){
    printf("Erreur lors des malloc du main\n");
    return EXIT_FAILURE;
  }
  
  /* Copie du nom de fichier */
  memcpy((void *)path_to_fic, (const void *)argv[1], sizeof(char)*100);
  memcpy((void *)path_to_fic2, (const void *)argv[2], sizeof(char)*100);
  // debug :  printf("path_to_fic = %s\n", path_to_fic);
  if(sizeof(path_to_fic) == 0){
    printf("%d : %s\n", errno, strerror(errno));    
    printf("Erreur lors du memcpy\n");
    return EXIT_FAILURE;
  }
  
  printf("Avant extraction\n");
  extraction_donnees(t_plaintexts, t_traces, path_to_fic);
  printf("Apres extraction\n");
  
  printf("Avant Calcul des Scores de Correlation\n");     
  score = Calcul_score(t_plaintexts, t_traces);
  printf("Apres Calcul des Scores de Correlation\n");

  Calcul_max_2D(score);
  
  printf(" Avant Ecriture dans le fichier de score\n");
  ecriture_score(score, path_to_fic2);
  printf(" Apres Ecriture dans le fichier de score\n");

  printf("Desallocation\n");
  for(i=256-1;i>=0;i--){
    free(score[i]);
  }
  free(score);
  free(path_to_fic);
  free(path_to_fic2);
  for(i=NB_MESURES-1;i>=0; i--){
    free(t_plaintexts[i]);
    free(t_traces[i]);
  }  
  free(t_plaintexts);
  free(t_traces);
  
  return EXIT_SUCCESS;
}

/* A FAIRE : 
- CHECK : Remplir la fonction qui calcule le poids de Hamming
- CHECK : Faire une relecture entiere du code (avec Nabil eventuellement)
- CHECK : Compiler et debuguer
- CHECK : Verifier pour Poids_Hamming la credibilité des valeurs obtenues
- CHECK : Verifier pour f_correlationCoefficient la credibilité des valeurs obtenues
- CHECK : Verifier pour Calcul_score la credibilité des valeurs obtenues
- CHECK : Faire la question 4 : 
  a) eventuellement faire un calcul de la colonne de score contenant
     les meilleurs resultats via un calcul de max de moyenne (sur les valeurs absolues) 
     (qui correspondra a l'instant de la mesure qui correspond le mieux au poids de hamming).
  b) eventuellement ajouter un calcul de la ligne de cette colonne contenant les meilleurs resultats 
    via un calcul de max (sur les valeurs absolues tjrs) par exemple 
    (qui sera la ligne ayant la cle qui est la plus correlee au mesure)
  c) Voir si on a le bon resultat, si c'est pas le cas,  inverser a et b
- A FAIRE: verifier si hbox calcule le bon truc

Refaire l'inversion a, b et voir les valeurs de correlation obtenus a mesure variable, cle fixe
REM : en faisant la selection sur les colonnes puis les lignes : on trouve score[??][???]
      en faisant la selection sur les lignes puis sur les colonnes : on trouve score[79][2080], cor=-0.38
 */

/* Questions :
   1/ Le poids de Hamming doit-il etre calcule sur le resultat de la SBox HW[S[v_keyHyp ^ t_plaintext[i]]],
      ce qui oblige a programmer la Sbox de l'AES, ou alors est-ce que l'on peut se limiter 
      a HW[v_keyHyp ^ t_plaintext[i]] ?
      -> on va se limiter a HW[v_keyHyp ^ t_plaintext[i]]
   2/ Le poids de Hamming est calcule sur v_keyHyp (1 octet)^ t_plaintext[i] (16octet).
      Des lors, faut-il faire plutot le calcul sur le 1er octet du plaintext ou sur le dernier ? 
      -> on va faire sur le 1er octet du plaintext : v_keyHyp[1,0]^t_plaintext[1,0]
*/
