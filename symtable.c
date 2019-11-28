//tabulka symbolu, predelana z ukolu c401 z IAL
//Zuzana Hradilova, xhradi16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "symtable.h"

		//funkce pro globalni tabulku symbolu
void GlobalSymTabInit (SymTabNodePtr *RootPtr) {
	*RootPtr = NULL;
}

//vyhledani promenne podle jejiho id
int GlobalSymTabSearch (SymTabNodePtr RootPtr, char * K, GlobalTableData **Obsah)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return GlobalSymTabSearch (RootPtr->LPtr, K, Obsah); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return GlobalSymTabSearch (RootPtr->RPtr, K, Obsah); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		*Obsah = RootPtr->Data; //vracim data o promenne
		return TRUE;
	}
}

int GlobalSymTabSearchMinus (SymTabNodePtr RootPtr, char * K)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return GlobalSymTabSearchMinus (RootPtr->LPtr, K); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return GlobalSymTabSearchMinus (RootPtr->RPtr, K); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		return TRUE;
	}
}

//vlozeni noveho symbolu do tabulky, musi bytint kvuli vraceni chyby
int GlobalSymTabInsert (SymTabNodePtr* RootPtr, char * K, GlobalTableData *Obsah){
	if (*RootPtr == NULL){ //pokud nebylo alokovane, alokuju
		if ((*RootPtr = malloc (sizeof(struct SymTabNode))) == NULL) return INTERNAL_ERROR; //vratim chybu
		(*RootPtr)->Key = K;
		(*RootPtr)->Data = Obsah;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
	}
	else if (K < (*RootPtr)->Key)
	    	GlobalSymTabInsert (&(*RootPtr)->LPtr, K, Obsah);
	else if (K > (*RootPtr)->Key)
	    	GlobalSymTabInsert (&(*RootPtr)->RPtr, K, Obsah);
	else (*RootPtr)->Data = Obsah; //strom toto id uz obsahuje -> prepisu obsah
	return OK;
}

//pomocna funkce pro nahrazeni
void ReplaceByRightmost (SymTabNodePtr PtrReplaced, SymTabNodePtr *RootPtr) {
	if (*RootPtr == NULL || PtrReplaced == NULL) return;
	if ((*RootPtr)->RPtr != NULL) ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
	else{
		PtrReplaced->Key = (*RootPtr)->Key;
		PtrReplaced->Data = (*RootPtr)->Data;
		SymTabNodePtr docasny  = (*RootPtr);
		(*RootPtr) = (*RootPtr)->LPtr;
		free(docasny);
	}
}

void LocalReplaceByRightmost (LocalTableNode PtrReplaced, LocalTableNode *RootPtr) {
	if (*RootPtr == NULL || PtrReplaced == NULL) return;
	if ((*RootPtr)->RPtr != NULL) LocalReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
	else{
		PtrReplaced->Key = (*RootPtr)->Key;
		PtrReplaced->localData = (*RootPtr)->localData;
		LocalTableNode docasny  = (*RootPtr);
		(*RootPtr) = (*RootPtr)->LPtr;
		free(docasny);
	}
}

//odstraneni symbolu s klicem k
void GlobalSymTabDelete (SymTabNodePtr *RootPtr, char * K) {
	if (*RootPtr == NULL) return;

	if ((*RootPtr)->Key < K) GlobalSymTabDelete((&(*RootPtr)->RPtr), K);
	else if ((*RootPtr)->Key > K) GlobalSymTabDelete((&(*RootPtr)->LPtr), K);

	else {
		if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr == NULL){ //uzel nema podstrom
			free (*RootPtr);
			*RootPtr = NULL;
		}
		else if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr != NULL){
			SymTabNodePtr docasny;
			docasny = (*RootPtr)->RPtr;
			free (*RootPtr);
			*RootPtr = docasny;
		}
		else if ((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr == NULL){
			SymTabNodePtr docasny;
			docasny = (*RootPtr)->LPtr;
			free (*RootPtr);
			*RootPtr = docasny;
		}
		else {
			if ((*RootPtr)->LPtr->RPtr != NULL) ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr->RPtr);
			else ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
		}
	}
}

//zruseni stromu
void GlobalSymTabDispose (SymTabNodePtr *RootPtr) {
	if (*RootPtr != NULL) {
		GlobalSymTabDispose(&(*RootPtr)->LPtr);
		GlobalSymTabDispose(&(*RootPtr)->RPtr);
		free (*RootPtr);
		*RootPtr = NULL;
	}
}

		//funkce pro lokalni tabulku symbolu
void LocalSymTabInit (LocalTableNode *RootPtr) {
	*RootPtr = NULL;
}

//vyhledani promenne podle jejiho id -- melo by byt hotove
int LocalSymTabSearch (LocalTableNode RootPtr, char * K, LocalTableData **Obsah)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return LocalSymTabSearch (RootPtr->LPtr, K, Obsah); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return LocalSymTabSearch (RootPtr->RPtr, K, Obsah); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		*Obsah = RootPtr->localData; //vracim data o promenne
		return TRUE;
	}
}

int LocalSymTabSearchMinus (LocalTableNode RootPtr, char * K)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return LocalSymTabSearchMinus (RootPtr->LPtr, K); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return LocalSymTabSearchMinus (RootPtr->RPtr, K); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		return TRUE;
	}
}

//vlozeni noveho symbolu do tabulky, musi bytint kvuli vraceni chyby
int LocalSymTabInsert (LocalTableNode* RootPtr, char * K, LocalTableData *Obsah){
	if (*RootPtr == NULL){ //pokud nebylo alokovane, alokuju
		if ((*RootPtr = malloc (sizeof(struct SymTabNode))) == NULL) return INTERNAL_ERROR; //vratim chybu
		(*RootPtr)->Key = K;
		(*RootPtr)->localData = Obsah;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
	}
	else if (K < (*RootPtr)->Key)
	    	LocalSymTabInsert (&(*RootPtr)->LPtr, K, Obsah);
	else if (K > (*RootPtr)->Key)
	    	LocalSymTabInsert (&(*RootPtr)->RPtr, K, Obsah);
	else (*RootPtr)->localData = Obsah; //strom toto id uz obsahuje -> prepisu obsah
	return OK;
}

//odstraneni symbolu s klicem k
void LocalSymTabDelete (LocalTableNode *RootPtr, char * K) {
	if (*RootPtr == NULL) return;

	if ((*RootPtr)->Key < K) LocalSymTabDelete((&(*RootPtr)->RPtr), K);
	else if ((*RootPtr)->Key > K) LocalSymTabDelete((&(*RootPtr)->LPtr), K);

	else {
		if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr == NULL){ //uzel nema podstrom
			free (*RootPtr);
			*RootPtr = NULL;
		}
		else if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr != NULL){
			LocalTableNode docasny;
			docasny = (*RootPtr)->RPtr;
			free (*RootPtr);
			*RootPtr = docasny;
		}
		else if ((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr == NULL){
			LocalTableNode docasny;
			docasny = (*RootPtr)->LPtr;
			free (*RootPtr);
			*RootPtr = docasny;
		}
		else {
			if ((*RootPtr)->LPtr->RPtr != NULL) LocalReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr->RPtr);
			else LocalReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
		}
	}
}

//zruseni stromu
void LocalSymTabDispose (LocalTableNode *RootPtr) {
	if (*RootPtr != NULL) {
		LocalSymTabDispose(&(*RootPtr)->LPtr);
		LocalSymTabDispose(&(*RootPtr)->RPtr);
		free (*RootPtr);
		*RootPtr = NULL;
	}
}

		//funkce pro seznam parametru

void ParamInit (ParamList * L) {
	L->first = NULL;
	L->act = NULL;
}

int ParamInsert(ParamList *L, char * id) {
  param * tmp = malloc (sizeof(struct param));
  param * last;

	if (tmp == NULL || L == NULL) return INTERNAL_ERROR;
	else {
	  	last = L->last;
		tmp->id = id;
		tmp->before = last;
		tmp->next = NULL;
		if (last != NULL) last->next = tmp;
		else L->first = tmp;
		L->last = tmp;
	}
  return OK;
}

// TODO
void ParamSucc(ParamList *L){
	return;
}

//hledani parametru
bool ParamSearch (ParamList *L, char * id){
	ParamFirst(L);
	while ((L->act->id != id)){
		ParamSucc(L);
		if (L->act == NULL) return FALSE; //došlo se na konec seznamu
	} return TRUE;
}

void DLDisposeList (ParamList *L) {
  if (L != NULL) {
    param * tmp;
    param * next;

    tmp = L->first;
  	while (tmp != NULL) {
      next = tmp->next;
  		free (tmp);
  		tmp = next;
  	}
    ParamInit (L);
  }
}

void ParamFirst (ParamList *L){
	L->act = L->first;
}

void DLSucc (ParamList *L) {
  if (L != NULL && L->act != NULL) {
    if (L->act != L->last) {
      L->act = L->act->next; //presunu aktivitu na nasledujici
    } else {
      L->act = NULL; //pokud byl aktivni posledni prvek -> neaktivni
    }
  }
}

char *ParamListGetActive(ParamList *L) {
    if (L != NULL) {
        if (L->act != NULL)
            return L->act->id;
        else
            return NULL;
    }
    else {
        return NULL;
    }
}

//nove pridane funkce
//nastavi hodnutu define na true
void SetDefine(SymTabNodePtr RootPtr, char * K){
	if (GlobalSymTabSearchMinus (RootPtr, K) == false) return;
	else RootPtr->Data->define = TRUE;
	return;
}

void SetParamCount (SymTabNodePtr RootPtr, char * K, int pocet){
	if (GlobalSymTabSearchMinus (RootPtr, K) == false) return;
	else RootPtr->Data->pocet_par = pocet;
	return;
}
// TODO spravit zvlast subor na test
// funkce pro testovani
/*
int main(int argc, char *argv[]){
	printf("Inicialuzuju a vlozim uzel: \n");

	SymTabNodePtr novynode;
	GlobalTableData nejakadata;
	
	char *Key = "nejaky kliiiiiiiiiic";
	nejakadata.type = TYPE_INT;
	nejakadata.funkce = TRUE;
	nejakadata.define = FALSE;
	
	char *OtherKey = "nejaky jiny klic";

	GlobalTableData jinadata;
	jinadata.type = TYPE_STRING;
	jinadata.funkce = FALSE;
	jinadata.define = TRUE;

	GlobalTableData *misto;


	GlobalSymTabInit(&novynode);
	GlobalSymTabInsert(&novynode, Key, &nejakadata);

	printf(" Klic: %s \n", novynode->Key);
	printf(" Data (datovy typ): %d \n", novynode->Data->type);
	printf(" Data (je funkce): %d \n", novynode->Data->funkce);
	printf(" Data (je definovano): %d \n \n", novynode->Data->define);

	printf("Vlozim dalsi uzel: \n");

	GlobalSymTabInsert(&novynode, OtherKey, &jinadata);
	printf(" Klic: %s \n", novynode->RPtr->Key);
	printf(" Data (datovy typ): %d \n", novynode->RPtr->Data->type);
	printf(" Data (je funkce): %d \n", novynode->RPtr->Data->funkce);
	printf(" Data (je definovano): %d \n \n", novynode->RPtr->Data->define);

	printf("zkusim hledat OtherKey: %d \n", (GlobalSymTabSearch(novynode, OtherKey, &misto)));
	char * NeexistujiciKlic = "nejsem tu";
	printf("zkusim hledat něco co tam neni: %d \n", (GlobalSymTabSearch(novynode, NeexistujiciKlic, &misto)));


	return 0;
}
*/
