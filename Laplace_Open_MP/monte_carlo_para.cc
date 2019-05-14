#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include  <mpi.h>

/*  Ecole des Mines de Saint-Étienne - avril 2018 *
 *  @ J. Bruchon, N. Moulin
 *  Toolbox CHP - UP MPI
 *
 * Résolution de Laplacien(u) = 0 par un algorithme de Monte-Carlo */

using namespace std;
/* TP Julien PELEGRI et Corentin GIROUD */

int main(int argc, char ** argv)
{
	MPI_Init (&argc ,&argv);

	double temps_init = MPI_Wtime();

	int rang, nb_procs;

	MPI_Comm_rank(MPI_COMM_WORLD,&rang);
	MPI_Comm_size(MPI_COMM_WORLD,&nb_procs);

	if ( argc < 2 )
	{
		if ( rang == 0 )
		  cout << "Il manque le nom du fichier de données !" << endl;
		exit(1);
        }

	int nx, ny, nb_tirages;

	// Lecture du fichier de données


	if ( rang == 0 )
	{
	ifstream fic_donnees;
	fic_donnees.open(argv[1], ifstream::in);

        fic_donnees >> nx >> ny >> nb_tirages;
	fic_donnees.close();
	}

	int i,j,l;

	if (rang==0)
	{
		for (i=1; i<nb_procs; i++)
		{
			MPI_Send(&nx,1,MPI_INT,i,1,MPI_COMM_WORLD);
			MPI_Send(&ny,1,MPI_INT,i,1,MPI_COMM_WORLD);
			MPI_Send(&nb_tirages,1,MPI_INT,i,1,MPI_COMM_WORLD);
		}
	}

	if (rang != 0)
	{
		MPI_Status status;
		MPI_Recv(&nx,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&ny,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&nb_tirages,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
	}

	// Affichage des données lues
  // On peut voir si on a bien lu les données

	if ( rang == 0 )
	{
		cout << "************************" << endl;
		cout << "Calcul exécuté sur " << nb_procs << " coeurs" << endl;
		cout << "Dimension selon x : " << nx << endl;
		cout << "Dimension selon y : " << ny << endl;
		cout << "Nombre de lancés par case : " << nb_tirages << endl;
	}

	// On va initialiser la grille

	double * grille = new double [nx*ny];
	int n=nx*ny;

	double valeurs_aux_bords[4] = {0,1,2,3}; // valeurs de la solution sur les bords y=0, x=nx, y=ny et x=0

	if ( rang == 0 )
	{
		for (i=0; i<nx*ny; i++)
			grille[i] = 0;  // Initialisation de la solution

		// Initialisation de la solution sur les bords

		for (i=0; i<nx; i++) // y=0
		  grille[i] = valeurs_aux_bords[0];

		for (i=0; i<ny; i++) // x=nx
		  grille[i*nx+nx-1] = valeurs_aux_bords[1];

		for (i=0; i<nx; i++) // y=ny
		  grille[ny*(nx-1)+i] = valeurs_aux_bords[2];

		for (i=0; i<ny; i++) // x=0
		  grille[i*nx] = valeurs_aux_bords[3];
	}


	if (rang==0)
	{
		for (i=1; i<nb_procs; i++)
		{
			MPI_Send(grille,n,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
		}
	}


	if (rang != 0)
	{
		MPI_Status status;
		MPI_Recv(grille,n,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&status);
	}

	srand(time(NULL));

	// on remplit la grille par tous les processeurs
	if (rang!=0)
	{
		l=0;
		while (l*(nb_procs-1)+rang<=ny-2)
		{
		  j=l*(nb_procs-1)+rang;
		  for (i=1; i<nx-1; i++)
		  {
        // chaque proc va prendre un certains nombre de particules
		    for (int n=0; n<nb_tirages; n++)
		    {
		      int position[2] = {i,j};
          // qd on voir un bord on passe à un
		      bool stop = 0;
		      double valeur = 0;
		      while ( stop == 0 )
		      {

			  int decision = rand() % 2;


			  if ( decision == 0 )
			    position[0]--;
			  else
			      position[0]++;

			  decision = rand() % 2;
			  if ( decision == 1 )
			    position[1]--;
			  else
			      position[1]++;


        // on regarde si on a un bords
			  if ( ( position[0] == 0 ) || ( position[0] == nx-1 ) || ( position[1] == 0 ) || ( position[1] == ny-1 ) )
			  {
			    valeur = grille[position[1]*nx + position[0]];
			    stop = 1;
			  }
		      }

		      grille[j*nx+i] += valeur;
		    }

      // pour avoir les solutions on fait la moyenne des tirages

	    grille[j*nx+i] /= nb_tirages;
	  }

	l++;
	}
	}
  // on envoie tous au proc zero pour finir le travail
	if (rang != 0)
	{
		MPI_Send(grille,n,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
	}

	else
		{
			int k;
			for (k=1; k<nb_procs; k++)
			{
				double* grille_aux = new double [nx*ny];
				MPI_Status status;
				MPI_Recv(grille_aux,n,MPI_DOUBLE,k,1,MPI_COMM_WORLD,&status);
				for (i=1; i<nx-1; i++)
				{
					for (j=1; j<ny-1; j++)
					{
						grille[j*nx+i] += grille_aux[j*nx+i];
					}
				}
			}

    // petit test pour verifier l'avancement
		//cout << "grille FINALE du proc "<< rang << endl;
		//printGrille(grille,nx,ny);
		}




	double temps_final = MPI_Wtime();
	cout << "temps final " << temps_final-temps_init << endl;
	// 2. Ecriture au format vtk (fait par le processeur 0)
	if ( rang == 0 ){
		cout << "Écriture du fichier vtk" << endl;
		stringstream nom;
		nom << "stochastique";
		int Nbnoe = nx*ny;
		double dx = 1.0/(nx-1);
		double dy = 1.0/(ny-1);

		nom << ".vtk" << '\0';
		ofstream fic(nom.str().c_str());

		fic << "# vtk DataFile Version 2.0" << endl;
		fic << "Laplacien stochastique" << endl;
		fic << "ASCII" << endl;
		fic << "DATASET STRUCTURED_POINTS" << endl;
		fic << "DIMENSIONS " << nx << "  " << ny << "  1 " << endl;
		fic << "ORIGIN 0 0 0" << endl;
		fic << "SPACING " << dx << "  " << dy << "  1" << endl;
		fic << "POINT_DATA " << Nbnoe << endl;
		fic << "SCALARS Concentration float" << endl;
		fic << "LOOKUP_TABLE default" << endl;
		for (i=0; i<Nbnoe; i++)
		{
		  fic << grille[i] << endl;
		}
		fic.close();
		delete [] grille;

		}
	MPI_Finalize ();
	return 0;
}

