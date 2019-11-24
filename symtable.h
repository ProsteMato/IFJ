/**
 * Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Tabulka symbolu, implementována pomocí binárního stromu
 * 
 * @author Zuzana Hradilova <xhradi16@stud.fit.vutbr.cz>
 * 
 */

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

/** bude upraveno
typedef struct par { //pareametry fce, ulozeny v seznamu
    char* nazev;	//ukazatel na id
		int poradi;		//poradi parametru
    struct par * next;	//dalsi parametr
} params; */

              //prototypy funkcí - globalni tabulka symbolu

//inicializace globalni tabulky symbolu
//@param tBSTNodePtr - ukazatel na tabulku symbolu
void GlobalSymTabInit (tBSTNodePtr *);

/** hledani symbolu podle klice
@param tBSTNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int GlobalSymTabSearch (tBSTNodePtr, char *, tData **);

/** vlozeni noveho symbolu
@param tBSTNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int GlobalSymTabInsert (tBSTNodePtr *, char *, tData *);

/** vymazani symbolu
@param tBSTNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void GlobalSymTabDelete (tBSTNodePtr *, char *);

/** zruseni tabulky symbolu
@param tBSTNodePtr - ukazatel na tabulku symbolu */
void GlobalSymTabDispose(tBSTNodePtr *);

              //prototypy funkcí - lokalni tabulka symbolu

//inicializace lokalni tabulky symbolu
//@param tBSTNodePtr - ukazatel na tabulku symbolu
void LocalSymTabInit (tBSTNodePtr *);

/** hledani symbolu podle klice
@param tBSTNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int LocalSymTabSearch (tBSTNodePtr, char *, tData **);

/** vlozeni noveho symbolu
@param tBSTNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int LocalSymTabInsert (tBSTNodePtr *, char *, tData *);

/** vymazani symbolu
@param tBSTNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void LocalSymTabDelete (tBSTNodePtr *, char *);

/** zruseni tabulky symbolu
@param tBSTNodePtr - ukazatel na tabulku symbolu */
void LocalSymTabDispose(tBSTNodePtr *);


///// TO DO ///
//funkce pro paramentry funkci
