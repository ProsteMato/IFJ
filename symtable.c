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
int GlobalSymTabSearch (SymTabNodePtr RootPtr, char * K, tData **Obsah)	{

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

//vlozeni noveho symbolu do tabulky, musi bytint kvuli vraceni chyby
int GlobalSymTabInsert (SymTabNodePtr* RootPtr, char * K, tData *Obsah){
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
void LocalSymTabInit (SymTabNodePtr *RootPtr) {
	*RootPtr = NULL;
}

//vyhledani promenne podle jejiho id -- melo by byt hotove
int LocalSymTabSearch (SymTabNodePtr RootPtr, char * K, tData **Obsah)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return LocalSymTabSearch (RootPtr->LPtr, K, Obsah); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return LocalSymTabSearch (RootPtr->RPtr, K, Obsah); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		*Obsah = RootPtr->Data; //vracim data o promenne
		return TRUE;
	}
}

//vlozeni noveho symbolu do tabulky, musi bytint kvuli vraceni chyby
int LocalSymTabInsert (SymTabNodePtr* RootPtr, char * K, tData *Obsah){
	if (*RootPtr == NULL){ //pokud nebylo alokovane, alokuju
		if ((*RootPtr = malloc (sizeof(struct SymTabNode))) == NULL) return INTERNAL_ERROR; //vratim chybu
		(*RootPtr)->Key = K;
		(*RootPtr)->Data = Obsah;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
	}
	else if (K < (*RootPtr)->Key)
	    	LocalSymTabInsert (&(*RootPtr)->LPtr, K, Obsah);
	else if (K > (*RootPtr)->Key)
	    	LocalSymTabInsert (&(*RootPtr)->RPtr, K, Obsah);
	else (*RootPtr)->Data = Obsah; //strom toto id uz obsahuje -> prepisu obsah
}

//odstraneni symbolu s klicem k
void LocalSymTabDelete (SymTabNodePtr *RootPtr, char * K) {
	if (*RootPtr == NULL) return;

	if ((*RootPtr)->Key < K) LocalSymTabDelete((&(*RootPtr)->RPtr), K);
	else if ((*RootPtr)->Key > K) LocalSymTabDelete((&(*RootPtr)->LPtr), K);

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
void LocalSymTabDispose (SymTabNodePtr *RootPtr) {
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
		tmp->nazev = id;
		tmp->before = last;
		tmp->next = NULL;
		if (last != NULL) last->next = tmp;
		else L->first = tmp;
		L->last = tmp;
	}
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
            return L->act->nazev;
        else
            return NULL;
    }
    else {
        return NULL;
    }
}