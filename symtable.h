/** Project: Implementace prekladace imperativniho jazyka IFJ19

 @brief Tabulka symbolu, implementovana pomoci binarniho stromu
 @author Zuzana Hradilova <xhradi16@stud.fit.vutbr.cz> */

#ifndef _SYM_TABLE_
#define _SYM_TABLE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//definice datovych typu pro ulozena data
typedef enum {
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_NONE,
  TYPE_UNDEFINED,
  TYPE_PARAM,
} Data_type;

// ulozeni paramentru funkce
typedef struct param {
    char * id;	//ukazatel na id
    struct param * before; //predchozi paramentr
    struct param * next;	//dalsi parametr
} param;

typedef struct paramlist {
  param *first;
  param *act;
  param *last;
} ParamList;

typedef struct LocalTableData {
  bool define;
  Data_type type;
} LocalTableData;

typedef struct LocalTableNode {
  char *Key;
  LocalTableData *localData;
  struct LocalTableNode *LPtr;
  struct LocalTableNode *RPtr;
} * LocalTableNode;

typedef struct GlobalTableData{
  Data_type type; //typ symbolu
  bool define;
  bool funkce; //funkce
  int pocet_par; //pouze u fce
  LocalTableNode *localTableNode;
  ParamList *paramList; //0. parametr (pouze u fce)
} GlobalTableData;

// definice pro binarni strom
typedef struct SymTabNode { // struktura definujici symbol
	char * Key; //id
	GlobalTableData * Data;
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
int GlobalSymTabSearch (SymTabNodePtr, char *, GlobalTableData **);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int GlobalSymTabInsert (SymTabNodePtr *, char *, GlobalTableData *);

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
void LocalSymTabInit (LocalTableNode *);

/** hledani symbolu podle klice
@param SymTabNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int LocalSymTabSearch (LocalTableNode, char *, LocalTableData **);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int LocalSymTabInsert (LocalTableNode *, char *, LocalTableData *);

/** vymazani symbolu
@param SymTabNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void LocalSymTabDelete (LocalTableNode *, char *);

/** zruseni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void LocalSymTabDispose (LocalTableNode *);

/*
// ulozeni paramentru funkce
typedef struct param {
    char * nazev;	//ukazatel na id
    struct param * before; //predchozi paramentr
    struct param * next;	//dalsi parametr
} param; */

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

//TODO funkce(paramlist, id) je tam
/** hleda parametr podle klici
@param ParamList - seznam parametru
@param char - klic ktery hledam
@return pokud je nalezeno vraci TRUE */
bool ParamSearch (ParamList *, char *);

/** nastavi hodnutu define na true
@param SymTabNodePtr - tabulka symbolu
@char - klic symbolu u ktereho mam provest zmenu */
void SetDefine(SymTabNodePtr, char *);

/** nastavi pocet paramentru
@param SymTabNodePtr - tabulka symbolu
@param char - ukazatel na klic
@param int - pocet parametru */
void SetParamCount (SymTabNodePtr, char *, int);

//hledani bez ulozeni dat
//vraci true, pokud je nalezeno
int GlobalSymTabSearchMinus (SymTabNodePtr, char *);

//hledani bez ulozeni dat
//vraci true, pokud je nalezeno
int LocalSymTabSearchMinus (LocalTableNode, char *);

#endif
