//tabulka symbolu, predelana z ukolu c401 z IAL
//Zuzana Hradilova, xhradi16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include "symtable.h"

//inicializace tabulky
void SymTabInitInit (tBSTNodePtr *RootPtr) {
	*RootPtr = NULL;
}

//vyhledani promenne podle jejiho id -- melo by byt hotove
int SymTabSearch (tBSTNodePtr RootPtr, char K, tData **Obsah)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return BSTSearch(RootPtr->LPtr, K, *Obsah); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return BSTSearch(RootPtr->RPtr, K, *Obsah); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		*Obsah = RootPtr->Data; //vracim data o promenne
		return TRUE;
	}
}

//vlozeni noveho symbolu do tabulky, musi bytint kvuli vraceni chyby
int SymTabInsert (tBSTNodePtr* RootPtr, char K, tData *Obsah){
	if (*RootPtr == NULL){ //pokud nebylo alokovane, alokuju
		if ((*RootPtr = malloc (sizeof(struct tBSTNode))) == NULL) return INTERNAL_ERROR; //vratim chybu
		(*RootPtr)->Key = K;
		(*RootPtr)->Data = Obsah;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
	}
	else if (K < (*RootPtr)->Key)
	    	BSTInsert (&(*RootPtr)->LPtr, K, Obsah);
	else if (K > (*RootPtr)->Key)
	    	BSTInsert (&(*RootPtr)->RPtr, K, Obsah);
	else (*RootPtr)->Data = Obsah; //strom toto id uz obsahuje -> prepisu obsah
}

//ulozeni parametru do seznamu


//pomocna funkce pro nahrazeni
void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
	if (*RootPtr == NULL || PtrReplaced == NULL) return;
	if ((*RootPtr)->RPtr != NULL) ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
	else{
		PtrReplaced->Key = (*RootPtr)->Key;
		PtrReplaced->Data = (*RootPtr)->Data;
		tBSTNodePtr docasny  = (*RootPtr);
		(*RootPtr) = (*RootPtr)->LPtr;
		free(docasny);
	}
}

//odstraneni symbolu s klicem k
void SymTabDelete (tBSTNodePtr *RootPtr, char K) {
	if (*RootPtr == NULL) return;

	if ((*RootPtr)->Key < K) BSTDelete((&(*RootPtr)->RPtr), K);
	else if ((*RootPtr)->Key > K) BSTDelete((&(*RootPtr)->LPtr), K);

	else {
		if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr == NULL){ //uzel nema podstrom
			free (*RootPtr);
			*RootPtr = NULL;
		}
		else if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr != NULL){
			tBSTNodePtr docasny;
			docasny = (*RootPtr)->RPtr;
			free (*RootPtr);
			*RootPtr = docasny;
		}
		else if ((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr == NULL){
			tBSTNodePtr docasny;
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
void SymTabDispose (tBSTNodePtr *RootPtr) {
	if (*RootPtr != NULL) {
		BSTDispose(&(*RootPtr)->LPtr);
		BSTDispose(&(*RootPtr)->RPtr);
		free (*RootPtr);
		*RootPtr = NULL;
	}
}
