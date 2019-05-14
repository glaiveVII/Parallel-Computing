#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include "Somme.h"

using namespace std;

int main(int argc, char ** argv)
{
	int i,N;
	N = 1;
	MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	double temps_debut, temps_fin;

	int taille;
	if ( argc > 1 )
        {
	   taille = atoi(argv[1]); // convertit un caractère en chiffre
           if ( rank == 0 )
		cout << "Taille : " << taille << endl;
        }
	else
        {
           if ( rank == 0 )
		cout << "Il manque la taille en argument" << endl;
	   exit(1);
        }

	// Définition de l'objet UneSomme de type Somme par appel au constructeur
	Somme UneSomme(taille); 

	UneSomme.Partitionnement();
	UneSomme.Sommation();

temps_debut = MPI_Wtime();
	for (i=0; i<N; i++)
	{
		UneSomme.Communication_pire();
	}
	temps_fin = MPI_Wtime();
	if ( rank == 0 )
		cout << "Temps de communication pour la plus mauvaise methode : "
			 << temps_fin-temps_debut << " s" << endl;
	UneSomme.Affichage();

/*
	temps_debut = MPI_Wtime();
	for (i=0; i<N; i++)
	{
		UneSomme.Communication_intermediaire();
	}
	temps_fin = MPI_Wtime();
	if ( rank == 0 )
		cout << "Temps de communication pour la methode intermediaire : "
			 << temps_fin-temps_debut << " s" << endl;
	UneSomme.Affichage();
*/
/*

	temps_debut = MPI_Wtime();
	for (i=0; i<N; i++)
	{
		UneSomme.Communication_optimale();
	}
	temps_fin = MPI_Wtime();
	if ( rank == 0 )
		cout << "Temps de communication pour la methode optimale : "
			 << temps_fin-temps_debut << " s" << endl;
	UneSomme.Affichage();
*/

/*
	temps_debut = MPI_Wtime();
	for (i=0; i<N; i++)
	{
		UneSomme.Communication_Reduce();
	}
	temps_fin = MPI_Wtime();
	if ( rank == 0 )
		cout << "Temps de communication pour la methode MPI_Allreduce : "
			 << temps_fin-temps_debut << " s" << endl;
	UneSomme.Affichage();

	MPI_Finalize();
*/
}
