#include <iostream>
#include <mpi.h>

using namespace std;

int main (int argc , char ** argv)
{
	MPI_Init(&argc, &argv);
	int total, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &total);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank==0)
	{
	 cout << "Programme s'exécutant sur " << total << "coeur(s) " << endl;
	}

	 cout << "Process " << rank << " : Hello World " << endl;
	
	MPI_Finalize();

	return 0;
} 
