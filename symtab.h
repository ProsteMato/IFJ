//tabulka symbolu, predelana z ukolu c401 z IAL
//Zuzana Hradilova, xhradi16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

//definice datovych typu pro ulozena data
typedef enum {
  TYPE_INT;
  TYPE_FLOAT;
  TYPE_STRING;
  TYPE_NONE; //nedefinovany
} data_type;


// definice pro binarni strom
typedef struct tBSTNode { // struktura definujici symbol
	char Key; //id
	tData Data;
	struct tBSTNode * LPtr; //levy podstrom
	struct tBSTNode * RPtr; //pravy podstrom
} *tBSTNodePtr;

typedef struct tData{
data_type Type; //typ symbolu
bool Define;
bool Global; //globalni promenna
bool Funkce; //funkce
  int pocet_par; //pouze u fce
  params *first; //0. parametr (pouze u fce)
}tData;

typedef struct par { //pareametry fce, ulozeny v seznamu
    char* nazev;	//ukazatel na id
		int poradi;		//poradi parametru
    struct par * next;	//dalsi parametr
} params;


// prototypy funkcí - binarni strom

void SymTabInit (tBSTNodePtr *); //inicializace tabulky symbolu
int SymTabSearch (tBSTNodePtr, char, tData *); //hledani symbolu podle klice
void SymTabInsert (tBSTNodePtr *, char, tData); //vlozeni noveho symbolu

//to do
void ParamInsert (tBSTNodePtr *, char, int); //ulozeni parametru
void BSTDelete (tBSTNodePtr *, char); //vymazani symbolu
void BSTDispose(tBSTNodePtr *); //zruseni tabulky symbolu
