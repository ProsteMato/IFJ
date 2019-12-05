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
  TYPE_BOOL,
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

// seznam parametru funkce
typedef struct paramlist {
  param *first;
  param *act;
  param *last;
} ParamList;

// tada symbolu v lokalni tabulce
typedef struct LocalTableData {
  bool define;
  Data_type type;
} LocalTableData;

// binarni strom lokalni tabulky
typedef struct LocalTableNode {
  char * Key;
  LocalTableData * localData;
  struct LocalTableNode * LPtr;
  struct LocalTableNode * RPtr;
} * LocalTableNode;

// tada symbolu v globalni tabulce
typedef struct GlobalTableData{
  Data_type type; //typ symbolu
  bool define;
  bool funkce; //funkce
  bool wasCalled;
  int pocet_par; //pouze u fce
  LocalTableNode *localTableNode;
  ParamList *paramList; //0. parametr (pouze u fce)
} GlobalTableData;

// binarni strom globalni tabulky
typedef struct SymTabNode { // struktura definujici symbol
	char * Key; //id
	GlobalTableData * Data;
	struct SymTabNode * LPtr; //levy podstrom
	struct SymTabNode * RPtr; //pravy podstrom
} * SymTabNodePtr;

// FUNKCE PRO GLOBALNI TABULKU SYMBOLU

/** inicializace globalni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void GlobalSymTabInit (SymTabNodePtr *);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int GlobalSymTabInsert (SymTabNodePtr *, char *, GlobalTableData *);

/** hledani symbolu podle klice
@param SymTabNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int GlobalSymTabSearch (SymTabNodePtr, char *, GlobalTableData **);

/** hledani bez ulozeni nalezenych dat
@param SymTabNodePtr - globalni tabulka symbolu
@param char - ukazatel na klic
@return vraci TRUE kdyz prvek s klicem v tabulce je, jinak FALSE */
int GlobalSymTabSearchMinus (SymTabNodePtr, char *);

/** vymazani symbolu z tabulky symbolu
@param SymTabNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void GlobalSymTabDelete (SymTabNodePtr *, char *);

/** zruseni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void GlobalSymTabDispose (SymTabNodePtr *);

/** nastavi hodnutu define na true
* @param SymTabNodePtr - tabulka symbolu
* @param char - klic symbolu u ktereho mam provest zmenu 
*/
void SetDefine(SymTabNodePtr, char *);

/**
 * @brief Nastaví premennú volania funckie na true
 * 
 * @param RootPtr pointer na globálnu tabuľku symbolov
 * @param K klúč ktorý bude hľadaný
 */
void SetCalled(SymTabNodePtr RootPtr, char * K);

/**
 * @brief skontroluje že či je funkcia volaná
 * 
 * @param RootPtr pointer na globálnu tabuľku symbolov
 * @param K klúč ktorý bude hľadaný
 * @return true ak funkcia bola volaná
 * @return false ak funkcia volaná nebola
 */
bool WasCalled(SymTabNodePtr RootPtr, char * K);

/** nastavi typ globalni premmennej
@param LocalTableNode - tabulka symbolu
@param char - ukazatel na klic
@param Data_type - datovy typ */
void GlobalSetType(SymTabNodePtr, char *, Data_type);

/** vraci lokalni tabulku symbolu patrici k funkci z globalni tabulky
@param SymTabNodePtr - tabulka symbolu
@param char - ukazatel na klic
@return pokud klic neni nalezen vraci NULL, jinak vraci lokalni tabulku symbolu */
LocalTableNode FindLocalTable(SymTabNodePtr, char *);

/** vrati param list funkce
@param SymTabNodePtr - tabulka symbolu
@param char - ukazatel na klic
@return pokud klic neni nalezen vraci NULL, jinak vraci list parametru */
ParamList * FindParamList(SymTabNodePtr, char *);

/** nastavi pocet paramentru
@param SymTabNodePtr - tabulka symbolu
@param char - ukazatel na klic
@param int - pocet parametru */
void SetParamCount (SymTabNodePtr, char *, int);

// FUNKCE PRO LOKALNI TABULKU SYMBOLU

/** inicializace lokalni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void LocalSymTabInit (LocalTableNode *);

/** vlozeni noveho symbolu
@param SymTabNodePtr - ukazatel na sym.tab. 
@param char - klic (id) vlozeneho symbolu 
@param tData - ukazatel na data 
@return v pripade neuspech alokace vraci chybu INTERNAL_ERROR */
int LocalSymTabInsert (LocalTableNode *, char *, LocalTableData *);

/** hledani symbolu podle klice
@param SymTabNodePtr - tabulka symbolu
@param char - klic (id) hledaneho symbolu
@param tData - misto kam se ulozi najdena data
@return vraci false pokud je hledani neuspesne */
int LocalSymTabSearch (LocalTableNode, char *, LocalTableData **);

/** hledani bez ulozeni nalezenych dat
@param SymTabNodePtr - lokalni tabulka symbolu
@param char - ukazatel na klic
@return vraci TRUE kdyz prvek s klicem v tabulce je, jinak FALSE */
int LocalSymTabSearchMinus (LocalTableNode, char *);

/** vymazani symbolu z tabulky symbolu
@param SymTabNodePtr - ukazatel na sym.tab.
@param char - klic (id) symbolu který má být smazán */
void LocalSymTabDelete (LocalTableNode *, char *);

/** zruseni tabulky symbolu
@param SymTabNodePtr - ukazatel na tabulku symbolu */
void LocalSymTabDispose (LocalTableNode *);

/** nastavi hodnutu define na true
@param SymTabNodePtr - tabulka symbolu
@char - klic symbolu u ktereho mam provest zmenu */
void LocalSetDefine(LocalTableNode, char *);

/** nastavi typ localni promenne
@param LocalTableNode - tabulka symbolu
@param char - ukazatel na klic
@param Data_type - datovy typ */
void LocalSetType(LocalTableNode, char *, Data_type);

// FUNKCE PRO SEZNAM PARAMETRU

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

/** nastaveni aktivitu na 1. prvek seznamu parametru
@param param - ukazatel seznam parametru */
void ParamFirst (ParamList *);

/** posune aktivitu na nasledujici prvek seznamu parametru
@param param - ukazatel seznam parametru */
void ParamSucc (ParamList *);

/** vraci id aktivniho prvku
@param param - ukazatel seznam parametru 
@return vraci id aktivniho prvku nebo NULL pokud neni aktivni */
char *ParamGetActive(ParamList *);

/** hleda parametr podle klici
@param ParamList - seznam parametru
@param char - klic ktery hledam
@return pokud je nalezeno vraci TRUE */
bool ParamSearch (ParamList *, char *);

/**TODO:
funkce ktera projde celou globalni tabulku a zkontroluje vsechny funkce, 
pokud najde nedefinovanou fci, vrati ukazatel prvni nedef. fce, jinak NULL */
char *UndefinedFunctionControl(SymTabNodePtr);

#endif
