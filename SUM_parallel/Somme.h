#ifndef _SOMME_H
#define _SOMME_H

class Somme
{
public:
	Somme(int taille);
	void Partitionnement();
	void Affichage();
	void Sommation();

	void Communication_pire();
	void Communication_intermediaire();
	void Communication_optimale();
	void Communication_Reduce();

//	Fonctions d'accès :
	void set_taille(int taille) { taille_totale = taille; }
	int get_taille() { return taille_totale; }

protected:
	int taille_totale;
	int debut_local;
	int fin_local;

	int nb_proc;
	int rank;

	double sum_p;
	double sum;
};

#endif
