#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>



// TP Julien PELEGRI et Corentin Giroud //

// Ici notre algorihtme a une compléxité en O(log(n)+n^2)=O(n^2)

// en effet la fusion se fait en O(log(n)) et le tri selection en O(n^2)


int rech_dich(double T[], int g, int d, double valeur)
{
	int m;
	if (g >= d) return g;
	m = (g+d) / 2;
	if (valeur < T[m])
		
		return rech_dich(T, g, m, valeur);
	else
		return rech_dich(T, m+1, d, valeur);
}


void rank(double t[], int n, double s[], int m, int *pos_t_s)
{
	int i;
	#pragma omp parallel for
		for (i=0; i < n; i++)
		{
			pos_t_s[i] = rech_dich(s,0,m,t[i]);
		}	
}


void fusion(double t[], int n, double s[], int m, double b[])
{
	int *pos_t_s;
	pos_t_s = malloc(n*sizeof(int));	

	int i;

	int *pos_s_t;
	pos_s_t = malloc(m*sizeof(int));	

	int x,y;

	rank(t,n,s,m, pos_t_s);
	rank(s,m,t,n, pos_s_t);

//	#pragma omp parallel private(tid) num_threads(n)
/*on a essayer avec cette methode, on avait un probleme sur le resultat qui etait faux */

	#pragma omp parallel for
		for (i=0; i<n; i++)
		{
			x = pos_t_s[i];		
			b[i + x] = t[i];	
		}

	#pragma omp parallel for
		for (i=0; i<m; i++)
		{
			y = pos_s_t[i];
		// si une valeur dans les deux listes est commune
		// si une valeur est commune plusieurs fois on fait une boucle while
		
		// on met -1 car on travail avec des positifs donc on ne peut avoir deux fois -1 
			if (b[i+y] == -1)
			// on a laisser ce test meme si la probabilité d'avoir deux nombres egaux est nulle
			{
				b[i+y] = s[i];
			}		
			else
			{
				b[i+y-1] = s[i];
			}
		}
}


void echange(double *T, int i, int j)
{
	double temp;
	temp = T[i];
	T[i] = T[j];
	T[j] = temp;
}

// on aurait du choisir un tri en O(log(n)*n) comme le tri bulle, plus rapide !
void tri_selection(double *T, int N)
{
	int k, j;
	int min;
	double valmin;

	for(k = 0; k < N-1; k++)
	{
		min = k; valmin = T[k];
		for (j = k+1; j < N; j++)
			if (T[j] < valmin)
			{
				min = j;
				valmin = T[j];
			}
		if (min != k)
			echange(T, k, min);
	}
}


void tri_fusion(double t[], int n, double b[]) 
{
    int m= n/2;
    double *t1;
    double *t2;
    int tid;
    int i;
    
    t1 = malloc(m*sizeof(double));
    t2 = malloc((n-m)*sizeof(double));
    
    for (i=0;i<m;i++)
    {
    	t1[i]=t[i];
    }
    for (i=0;i<n-m;i++)
    {
    	t2[i]=t[i+m];
    }
    
    // première facon de faire pour paralléliser
    // pour separer sur deux processeurs
    /*#pragma omp parallel default(shared) private(tid) num_threads(2)
    { 
	    tid = omp_get_thread_num();
	    if(tid == 0)
		{
			tri_selection(t1, m);
		}       
		else
		{
		    tri_selection(t2, n-m);
		}
    }*/

    // deuxième facon de faire pour paralléliser
    // Cette methode est plus lente 
    #pragma omp parallel sections
    { 
       #pragma omp section
       tri_selection(t1, m);
       #pragma omp section
       tri_selection(t2, n-m);
    }

    fusion(t1, m, t2, n-m, b);
}

int main()
{
	clock_t v_debut, v_fin;
	float duree;

	int n = 10; /*taille du tableau à trier*/
	# define A 10
	
	// allocation dynamique pour optimiser l'espace mémoire
	double *t;
	t = malloc(n*sizeof(double));
	
	double *b;
	b = malloc(n*sizeof(double));
	
	int i;

	for (i=0; i<n ; i++)
	{
		t[i] = ((double)rand()/RAND_MAX) * A;

		//t[i] = rand() % A;
	}

	for (i=0;i<n;i++)
	{
		b[i] = -1;
	}
	
	printf("Tableau non trié : ");
	for (i = 0; i < n; i++)
	{ 
		printf("%f ", t[i]);
	}
	printf("\n");

	v_debut = clock();
	tri_fusion(t,n,b);
	v_fin = clock();


	duree = (float)(v_fin - v_debut) /CLOCKS_PER_SEC;
	
	printf("Tabeau trié : ");
	for (i = 0; i < n; i++)
	{ 
		printf("%f ", b[i]);
	}
	printf("\n");
	
	// en test sur des listes de taille 1000 cela permet de voir l'influence du parallelisme

	printf("Le temps mis pour calculer est de (en seconde) %f: ", duree);
	
    exit(0);

}



