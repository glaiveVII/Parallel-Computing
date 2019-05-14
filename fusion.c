#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// TP Julien PELEGRI et Corentin Giroud //

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
			{
				b[i+y] = s[i];
			}		
			else
			{
				b[i+y-1] = s[i];
			}
		}
}


int main()
{
	int n = 5;
	int m = 5;
	# define A 10
	double *t;
    double *s;
    double *b;
	t = malloc(n*sizeof(double));
	s = malloc(m*sizeof(double));
	b = malloc((n+m)*sizeof(double));

	int i;

	for (i=0;i<n;i++)
	{
		t[i] = i+rand()*1./RAND_MAX;
		
		// autre facon de générer mais ici plus dure on a des cas d'égalité 
		//t[i] = rand() % A;
	}
	
	for (i=0;i<m;i++)
	{
		// on ajoute un petit nombre plus petit que 1 pour eviter d'avoir des nombres egaux 
		s[i] = i+rand()*1./RAND_MAX;
	}
	
	for (i=0;i<m+n;i++)
	{
		b[i] = -1;
	}
	
	printf("Premier tableau trié t : ");
	for (i = 0; i < n; i++)
	{ 
		printf("%f ", t[i]);
	}
	printf("\n");

	printf("Deuxieme tableau trié s : ");
	for (i = 0; i < m; i++)
	{ 
		printf("%f ", s[i]);
	}
	printf("\n");

	fusion(t, n, s, m, b);

	printf("Tableau fusionnant t et s trié : ");
	for (i = 0; i < n+m; i++)
	{ 
		printf("%f ", b[i]);
	}
	printf("\n");
	
    exit(0);

}



