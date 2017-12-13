#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//typedef struct Arbre Arbre;
struct Arbre
{
	struct Arbre* gauche;
	struct Arbre* droit;
	int poids;
	int lettre;
};

enum Bit
{
	ZERO,
	UN
};

int lectureFichier(char* nomFichier, int* tab){
	FILE* f = fopen(nomFichier, "r");
	int nbCarac = 0;

	if (f == NULL){
		printf("Erreur lecture fichier %s",nomFichier);
		exit(0);
	}

	int caractereActuel = 0;
	do
    {
		caractereActuel = fgetc(f); // On lit le caractère
		if(caractereActuel != EOF){
			tab[caractereActuel]++;
			nbCarac ++;
		}
	} while (caractereActuel != EOF);

	fclose(f);
	return nbCarac;
}

struct Arbre** construireFeuilles(int *tab){

	int i =0;
	struct Arbre** tableauArbre = malloc(255*sizeof(struct Arbre*));
	for(i=0;i<255;i++){
		tableauArbre[i] = NULL;
	}
	for(i=0;i<255;i++){
		if(tab[i]>0){
			struct Arbre* n = malloc(sizeof(struct Arbre));
			n->gauche = NULL;
			n->droit = NULL;
			n->poids = tab[i];
			n->lettre = i;
			tableauArbre[i] = n;
		}
	}
	return tableauArbre;
}

int tailleTableauArbre(struct Arbre** tab){
	int taille = 0;
	int i = 0;
	for(i=0;i<255;i++){
		if(tab[i] != NULL){
			taille ++;
		}
	}
	return taille;
}

struct Arbre* construireArbre(struct Arbre** tab){

	while(tailleTableauArbre(tab) > 1){
		int poidsMin1 = 0;
		int poidsMin2 = 0;

		int indiceArbre1 = -1;
		int indiceArbre2 = -1;
		int i =0;

		for(i=0;i<255;i++){
			if(tab[i]!=NULL){
				int poids = tab[i]->poids;
				if(poidsMin1 == 0){
					poidsMin1 = poids;
					indiceArbre1 = i;
				}
				else if(poidsMin2 == 0){
					if (poids >= poidsMin1){
						poidsMin2 = poids;
						indiceArbre2 = i;
					}
					else {
						poidsMin2 = poidsMin1;
						indiceArbre2 = indiceArbre1;
						poidsMin1 = poids;
						indiceArbre1 = i;
					}
				}
				else {
					if(poids < poidsMin2){
						if(poids < poidsMin1){
							poidsMin2 = poidsMin1;
							indiceArbre2 = indiceArbre1;
							poidsMin1 = poids;
							indiceArbre1 = i;
						}
						else{
							poidsMin2 = poids;
							indiceArbre2 = i;
						}
					}
				}
			}
		}
		struct Arbre* n = malloc(sizeof(struct Arbre));
		n->gauche = tab[indiceArbre1];
		n->droit = tab[indiceArbre2];
		n->poids = poidsMin1 + poidsMin2;
		n->lettre = -1;
		tab[indiceArbre1] = n;
		tab[indiceArbre2] = NULL;
	}
	struct Arbre* racine = NULL;
	int i =0;
	for(i=0;i<255;i++){
		if(tab[i]!=NULL){
			racine = tab[i];
		}
	}

	return racine;
}

void construireDictionnaireRec(struct Arbre* racine, \
	enum Bit** tab, enum Bit* chemin, int tailleChemin, \
	int* taille){

	if(racine->gauche == NULL && racine->droit == NULL){
		tab[racine->lettre] = chemin;
		taille[racine->lettre] = tailleChemin;
	}
	else{
		enum Bit* cheminGauche = \
			malloc((tailleChemin+1)*sizeof(enum Bit));
		enum Bit* cheminDroit = \
			malloc((tailleChemin+1)*sizeof(enum Bit));
		int i = 0;
		for (i=0;i<tailleChemin;i++){
			cheminGauche[i] = chemin[i];
			cheminDroit[i] = chemin[i];
		}
		if(chemin != NULL){
			free(chemin);
		}
		cheminGauche[tailleChemin] = ZERO;
		cheminDroit[tailleChemin] = UN;
		construireDictionnaireRec(racine->gauche, 
			tab, cheminGauche, tailleChemin+1, taille);
		construireDictionnaireRec(racine->droit, tab, 
			cheminDroit, tailleChemin+1, taille);
	}
}

enum Bit** construireDictionnaire(struct Arbre* racine, int* taille){
	enum Bit** tab = malloc(255*sizeof(enum Bit*));
	int i =0;
	for(i=0;i<255;i++){
		tab[i] = NULL;
	}

	enum Bit* chemin = NULL;

	construireDictionnaireRec(racine, tab, chemin, 0, taille);

	return tab;
}

void libererArbre(struct Arbre* racine){
	if(racine->gauche==NULL && racine->droit==NULL){
		free(racine);
	}
	else{
		libererArbre(racine->gauche);
		libererArbre(racine->droit);
		free(racine);
	}
}

char transformerBitsEnCaractere(enum Bit* tab){
	int carac = 0;
	int i = 0;
	for(i=0; i<7;i++){
		if(tab[i]==UN){
			carac += pow(2,6-i);
		}
	}
	return carac;
}

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

void ecrireEntete(FILE* fichier, enum Bit** tab, int* taille, int nbElemEntete){
	fprintf(fichier,"_	%d\n",nbElemEntete);
	int i =0, j=0, k=0;
	for(i=0;i<255;i++){
		if(tab[i]!=NULL){
			char buff[10];
			int nbCarac = 0;
			enum Bit bits[7];
			int nbBits = 0, l=0;
			for(j=0;j<10;j++){
				buff[j] = '\0';
			}
			for(j=0;j<taille[i];j++){
				bits[nbBits]=tab[i][j];
				nbBits++;
				if(nbBits == 7){
					char carac = transformerBitsEnCaractere(bits);
					enum Bit* buff2 = transformerCaractereEnBits(carac);
					for(k=0;k<7;k++){
						if(bits[k] != buff2[k]){
							char buff3[8];
							char buff4[8];
							buff3[7] = '\0';
							buff4[7] = '\0';
							for(l=0;l<7;l++){
								if(bits[l]==UN){
									buff3[l]='1';
								}
								else{
									buff3[l]='0';
								}
								if(buff2[l]==UN){
									buff4[l]='1';
								}
								else{
									buff4[l]='0';
								}
							}
							printf("Erreur encodage de bits |%c| %s %s\n", carac, buff3, buff4);
							exit(0);
						}
					}
					free(buff2);
					nbBits = 0;
					buff[nbCarac] = carac;
					nbCarac++;
				}
			}
			if(nbBits != 0){
				for(j=nbBits;j<7;j++){
					bits[j] = UN;
				}
				char carac = transformerBitsEnCaractere(bits);
				enum Bit* buff2 = transformerCaractereEnBits(carac);
				for(k=0;k<7;k++){
					if(bits[k] != buff2[k]){
						char buff3[8];
						char buff4[8];
						buff3[7] = '\0';
						buff4[7] = '\0';
						for(l=0;l<7;l++){
							if(bits[l]==UN){
								buff3[l]='1';
							}
							else{
								buff3[l]='0';
							}
							if(buff2[l]==UN){
								buff4[l]='1';
							}
							else{
								buff4[l]='0';
							}
						}
						printf("Erreur encodage de bits |%c| %s %s\n", carac, buff3, buff4);
						exit(0);
					}
				}
				free(buff2);
				buff[nbCarac] = carac;
			}				

			
			fprintf(fichier, "%c	%d	%s\n",i,taille[i],buff);
		}
	}
}

void ecrireFichierCompresse(char* nomFichier, FILE* fo, enum Bit** tab, int* taille){

	FILE* fi = fopen(nomFichier,"r");

	if(fi==NULL){
		printf("Erreur ouverture fichier %s en lecture\n", nomFichier);
		exit(0);
	}

	int caractereActuel = 0;
	enum Bit bits[7];
	int nbBits = 0;
	int i = 0;
	do
    {
		caractereActuel = fgetc(fi); // On lit le caractère
		if(caractereActuel != EOF){
			for(i=0;i<taille[caractereActuel];i++){
				bits[nbBits] = tab[caractereActuel][i];
				nbBits++;
				if(nbBits == 7){
					char carac = transformerBitsEnCaractere(bits);
					nbBits = 0;
					fputc(carac,fo);
				}
			}
		}
	} while (caractereActuel != EOF);
	int nbBitsEnTrop = 0;
	if(nbBits!=0){
		nbBitsEnTrop = 7-nbBits;
		for(i=nbBits;i<7;i++){
			bits[i] = ZERO;
		}
		char carac = transformerBitsEnCaractere(bits);		
		fputc(carac,fo);
	}

	fclose(fi);

	rewind(fo);
	fprintf(fo,"%d",nbBitsEnTrop);
}

int main(int argc, char* argv){
	struct Arbre arbre = {NULL, NULL, 0, 0};

	int tab[255];
	int i = 0;
	for (i=0;i<255;i++){
		tab[i]=0;
	}

	char* nomFichier = "texte.txt";

	int nbCarac = lectureFichier(nomFichier,tab);
	//printf("%d\n", nbCarac);
	/*
	for(i=0;i<255;i++){
		printf("%c : %d\n", i, tab[i]);
	}
	*/
	struct Arbre** tableauArbre = construireFeuilles(tab);
	/*
	for(i=0;i<255;i++){
		if(tableauArbre[i] != NULL){
			printf("%c : %d\n", tableauArbre[i]->lettre, \
				tableauArbre[i]->poids);
		}
	}
	*/

	struct Arbre* arb = construireArbre(tableauArbre);
	//printf("%d\n", arb->poids);

	int* taille = malloc(255*sizeof(int));
	for(i=0;i<255;i++){
		taille[i] = 0;
	}
	enum Bit** tableauBit = construireDictionnaire(arb, taille);
	int j = 0;

	int nbElemEntete = 0;
	
	for(i=0;i<255;i++){
		if(tableauBit[i] != NULL){
			nbElemEntete++;
			char buff[100];
			for(j=0;j<100;j++){
				buff[j]='\0';
			}
			for(j=0;j<taille[i];j++){
				if(tableauBit[i][j] == ZERO){
					buff[j] = '0';
				}
				else{
					buff[j] = '1';
				}
			}
			printf("%c : %s\n", i, buff);
		}
	}
	
	//free(taille);
	libererArbre(arb);
	free(tableauArbre);
	
	FILE* fo = fopen("fichierCompresse.txt","w"); 

	if(fo == NULL){
		printf("Impossible d'ouvrir le fichier");
		exit(0);
	}

	ecrireEntete(fo, tableauBit, taille, nbElemEntete);
	
	ecrireFichierCompresse(nomFichier, fo, tableauBit, taille);

	fclose(fo);

	return 0;
}
