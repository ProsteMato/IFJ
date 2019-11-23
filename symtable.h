//tabulka symbolu, predelana z ukolu c401 z IAL
//Zuzana Hradilova, xhradi16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

//definice datovych typu pro ulozena data
typedef enum {
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_NONE,
  TYPE_UNDEFINED,
} data_type;


// definice pro binarni strom
typedef struct tBSTNode { // struktura definujici symbol
	char * Key; //id
	tData * Data;
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

//inicializace tabulky symbolu
//@param tBSTNodePtr - ukazatel na tabulku symbolu
void SymTabInit (tBSTNodePtr *);

/** hledani symbolu podle klice
@param tBSTNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu (bude upraveno na ukazatel klůli delce)
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int SymTabSearch (tBSTNodePtr, char, tData **);

/** vlozeni noveho symbolu
@param tBSTNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu (bude upraveno na ukazatel klůli delce) 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int SymTabInsert (tBSTNodePtr *, char, tData *);

/** vymazani symbolu
@param tBSTNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán (bude upraveno na ukazatel klůli delce) */
void SymTabDelete (tBSTNodePtr *, char);

/** zruseni tabulky symbolu
@param tBSTNodePtr - ukazatel na tabulku symbolu */
void SymTabDispose(tBSTNodePtr *);

///// TO DO ///

// vsechny funkce budou rozděleny na globalni a lokalni, parametry by meli pro většinu zustat stejne

//predelat id (klic) na ukazovatel
//funkce pro paramentry
//rozdelit na globalni a lokalni
