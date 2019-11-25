/** Project: Implementace prekladace imperativniho jazyka IFJ19

 @brief Tabulka symbolu, implementovana pomoci binarniho stromu
 @author Zuzana Hradilova <xhradi16@stud.fit.vutbr.cz> */

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

// ulozeni paramentru funkce
typedef struct param {
    char * nazev;	//ukazatel na id
    struct param * before; //predchozi paramentr
    struct param * next;	//dalsi parametr
} param;

typedef struct tData{
data_type Type; //typ symbolu
bool Define;
bool Global; //globalni promenna
bool Funkce; //funkce
  int pocet_par; //pouze u fce
  param * first; //0. parametr (pouze u fce)
}tData;

// definice pro binarni strom
typedef struct SymTabNode { // struktura definujici symbol
	char * Key; //id
	tData * Data;
	struct SymTabNode * LPtr; //levy podstrom
	struct SymTabNode * RPtr; //pravy podstrom
} * SymTabNodePtr;



              //prototypy funkcí - globalni tabulka symbolu

//inicializace globalni tabulky symbolu
//@param SymTabNodePtr - ukazatel na tabulku symbolu
void GlobalSymTabInit (SymTabNodePtr *);

/** hledani symbolu podle klice
@param SymTabNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int GlobalSymTabSearch (SymTabNodePtr, char *, tData **);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int GlobalSymTabInsert (SymTabNodePtr *, char *, tData *);

/** vymazani symbolu
@param SymTabNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void GlobalSymTabDelete (SymTabNodePtr *, char *);

/** zruseni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void GlobalSymTabDispose (SymTabNodePtr *);

              //prototypy funkcí - lokalni tabulka symbolu

//inicializace lokalni tabulky symbolu
//@param SymTabNodePtr - ukazatel na tabulku symbolu
void LocalSymTabInit (SymTabNodePtr *);

/** hledani symbolu podle klice
@param SymTabNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int LocalSymTabSearch (SymTabNodePtr, char *, tData **);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int LocalSymTabInsert (SymTabNodePtr *, char *, tData *);

/** vymazani symbolu
@param SymTabNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void LocalSymTabDelete (SymTabNodePtr *, char *);

/** zruseni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void LocalSymTabDispose (SymTabNodePtr *);

/*
// ulozeni paramentru funkce
typedef struct param {
    char * nazev;	//ukazatel na id
    struct param * before; //predchozi paramentr
    struct param * next;	//dalsi parametr
} param; */

typedef struct paramlist {
        param *first;
        param *act;
        param *last;
    } ParamList;

              //prototypy funkcí - seznam parametru funkce

/** inicializace seznamu parametru
@param param - ukazatel seznam parametru */
void ParamInit (ParamList *);

/** vlozeni parametru na seznam, parametr bude vlozen na konec 
@param param - ukazatel na seznam parametru
@param char - klic (id) paramentru 
@return v pripade neupechu alokace vraci chybu 99 */
int ParamInsert (ParamList *, char *);

/** zruseni seznamu parametru
@param param - ukazatel seznam parametru */
void ParamDispose (ParamList *);

/** nastaveni aktivity na 1. prvek seznamu parametru
@param param - ukazatel seznam parametru */
void ParamFirst (ParamList *);

/** posune aktivitu na nasledujici prvek seznamu parametru
@param param - ukazatel seznam parametru */
void ParamSucc (ParamList *);

/** vraci id aktivniho prvku
@param param - ukazatel seznam parametru 
@return vraci id aktivniho prvku nebo NULL pokud neni aktivni */
char *ParamGetActive(ParamList *);