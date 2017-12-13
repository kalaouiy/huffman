#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct Arbre
{
	struct Arbre* gauche;
	struct Arbre* droit;
	int lettre;
};

enum Bit
{
	ZERO,
	UN
};

enum Bit* transformerCaractereEnBits(char caractere){
	enum Bit* tab = malloc(7*sizeof(enum Bit));
	int carac = caractere;
	int j =0;
	for(j=0;j<7;j++){
		if(carac >= pow(2, 6-j)){
			tab[j] = UN;
			carac -= pow(2, 6-j); 
		}
		else{
			tab[j] = ZERO;
		}
	}
	return tab;
}

int recupereEntier(FILE* fichier){
	char buff[10];
	int i = 0;
	int nbCarac = 0;
	for(i=0;i<10;i++){
		buff[i] = '\0';
	}
	int caractere;
	do {
		caractere = fgetc(fichier);
		if(caractere != '	' && caractere != '\n'){
			buff[nbCarac] = caractere;
			nbCarac++;
		}
	} while(caractere != '	' && caractere != '\n');

	int size = strlen(buff);
	int retour = 0;
	for(i=size-1;i>=0;i--){
		retour += (buff[i] - '0')*pow(10,size-1-i);
	}
	return retour;
}



void construireChemin(enum Bit* chemin, int nbBits, \
	int nbCarac, char* buff){
	int i = 0;
	int j = 0;
	int numBit = 0;

	for(i=0;i<nbCarac;i++){
		int caractere = buff[i];

		for(j=6;j>=0;j--){
			if(numBit<nbBits){
				if(caractere>=pow(2,j)){
					chemin[numBit]=UN;
					caractere = caractere - pow(2,j);
				}
				else{
					chemin[numBit]=ZERO;	
				}
				numBit++;
			}
		}
	}
}


void insererElemEnteteArbreRec(struct Arbre* arbre, \
	enum Bit* chemin, int caractere, int nbBits){

	if(chemin[0]==ZERO){
		if(arbre->gauche==NULL){
			arbre->gauche = malloc(sizeof(struct Arbre));
			arbre->gauche->gauche = NULL;
			arbre->gauche->droit = NULL;
			arbre->gauche->lettre = -1;
		}

		if(nbBits == 1){
			arbre->gauche->lettre = caractere;
			printf("Insertion caractere %c\n", caractere);
		}
		else{
			insererElemEnteteArbreRec(arbre->gauche, \
				(chemin+1), caractere,nbBits-1);
		}
	}
	else{
		if(arbre->droit==NULL){
			arbre->droit = malloc(sizeof(struct Arbre));
			arbre->droit->gauche = NULL;
			arbre->droit->droit = NULL;
			arbre->droit->lettre = -1;
		}

		if(nbBits == 1){
			arbre->droit->lettre = caractere;
			//printf("Insertion caractere %c\n", caractere);
		}
		else{
			insererElemEnteteArbreRec(arbre->droit, \
				(chemin+1), caractere,nbBits-1);
		}
	}
}

void insererElemEnteteArbre(struct Arbre* arbre, \
	int carac, int nbBits, int nbCarac, char* buff){

	enum Bit chemin[nbBits];
	construireChemin(chemin, nbBits, nbCarac, buff);

	char buff2[nbBits+1];
	int i =0;
	for(i=0;i<nbBits;i++){
		if(chemin[i]==ZERO){
			buff2[i] = '0';
		}
		else{
			buff2[i] = '1';
		}
	}
	buff2[nbBits]='\0';

	//printf("Caractere : %c, Chemin : %s\n", carac, buff2);

	insererElemEnteteArbreRec(arbre, chemin, carac, nbBits);
}

int obtenirEntete(FILE* fichier, struct Arbre* arbre){
	int nbBitsEnTrop = recupereEntier(fichier);
	int nbEntetes = recupereEntier(fichier);
	int i = 0;
	int j=0;
	for(i=0;i<nbEntetes;i++){
		int carac = fgetc(fichier);
		fgetc(fichier);
		int nbBits = recupereEntier(fichier);

		int nbCarac = (nbBits/7);
		if(nbBits % 7 != 0){
			nbCarac += 1;
		}
		char encodage[nbCarac+1];
		encodage[nbCarac] = '\0';
		for (j=0;j<nbCarac;j++){
			encodage[j] = fgetc(fichier);
		}
		fgetc(fichier);

		//printf("%c : %d : %d : %s\n", carac, nbCarac, \
			nbBits, encodage);

		insererElemEnteteArbre(arbre, carac, nbBits, \
			nbCarac, encodage);
	}

	return nbBitsEnTrop;
}


void afficherArbreRec(struct Arbre* arbre, char* chemin){
	if(arbre->gauche == NULL && arbre->droit == NULL){
		printf("Caractere : %c, chemin : %s\n", \
			arbre->lettre,chemin);
	}
	else if(arbre->gauche == NULL || arbre->droit == NULL){
		printf("Erreur parcours arbre\n");
		exit(0);
	}
	else{
		if(arbre->gauche != NULL){
			int size = strlen(chemin);
			char buff[size+2];
			int i=0;
			for(i=0;i<size;i++){
				buff[i]=chemin[i];
			}
			buff[size] = '0';
			buff[size+1] = '\0';
			afficherArbreRec(arbre->gauche, buff);
		}
		if(arbre->droit != NULL){
			int size = strlen(chemin);
			char buff[size+2];
			int i=0;
			for(i=0;i<size;i++){
				buff[i]=chemin[i];
			}
			buff[size] = '1';
			buff[size+1] = '\0';
			afficherArbreRec(arbre->droit, buff);
		}
	}
}

int ecrireCaracteres(FILE* fo, struct Arbre* racine, enum Bit* buff, \
	int nbBits, int nbBitsEnTrop){

	int iBit = 0;
	int indiceDernierBitAnalyse = 0;

	struct Arbre* parcoursArbre = racine;

	for(iBit = 0; iBit<(nbBits-nbBitsEnTrop);iBit++){
		if(buff[iBit]==ZERO){
			parcoursArbre = parcoursArbre->gauche;
		}
		else{
			parcoursArbre = parcoursArbre->droit;
		}
		if(parcoursArbre==NULL){
			printf("Erreur parcours arbre\n");
			exit(0);
		}

		if(parcoursArbre->gauche==NULL && parcoursArbre->droit==NULL){
			char lettre = parcoursArbre->lettre;
			fputc(lettre, fo);
			parcoursArbre = racine;
			indiceDernierBitAnalyse = iBit;
		}
	}

	for(iBit=indiceDernierBitAnalyse+1;iBit<nbBits;iBit++){
		buff[iBit-indiceDernierBitAnalyse-1] = buff[iBit];
	}
	return (nbBits-(indiceDernierBitAnalyse+1));
}

void ecrireFichier(FILE* fi, FILE* fo, struct Arbre* racine,\
	 int nbBitsEnTrop){
	

	int tailleMaxEncodage = 30;

	enum Bit buffBit[tailleMaxEncodage+6];
	int nbBits = 0;

	int caractere = -1;
	do {
		caractere = fgetc(fi);
		if(caractere != EOF){
			enum Bit* buff = transformerCaractereEnBits(caractere);
			int i = 0;
			for(i=0;i<7;i++){
				buffBit[nbBits] = buff[i];
				nbBits++;
			}
			free(buff);
			if(nbBits > tailleMaxEncodage){
				nbBits = ecrireCaracteres(fo, racine, buffBit, \
					nbBits, nbBitsEnTrop);
			}
		}
	} while(caractere != EOF);
	nbBits = ecrireCaracteres(fo, racine, buffBit, \
		nbBits, nbBitsEnTrop);

	if(nbBits != nbBitsEnTrop){
		printf("Erreur ecriture fichier, le nombre de bits \
			restant ne correspond pas : %d %d\n",nbBitsEnTrop, nbBits);
		exit(0);
	}
}

int main(int argc, char* argv){

	char* fichierCompresse = "fichierCompresse.txt";
	char* fichierDecompresse = "fichierDecompresse.txt";

	FILE* fi = fopen(fichierCompresse, "r");

	if(fi==NULL){
		printf("Erreur ouverture fichier %s en lecture\n", 
			fichierCompresse);
		exit(0);
	}

	struct Arbre* arbre = malloc(sizeof(struct Arbre));
	arbre->gauche = NULL;
	arbre->droit = NULL;
	arbre->lettre = -1;

	int nbBitsEnTrop = obtenirEntete(fi, arbre);

	afficherArbreRec(arbre, "\0");

	FILE* fo = fopen(fichierDecompresse, "w");

	ecrireFichier(fi, fo, arbre, nbBitsEnTrop);

	fclose(fo);
	fclose(fi); 
}