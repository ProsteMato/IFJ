//tabulka symbolu, predelana z ukolu c401 z IAL
//Zuzana Hradilova, xhradi16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"

//inicializace tabulky
void SymTabInitInit (tBSTNodePtr *RootPtr) {
	*RootPtr = NULL;
}

//vyhledani promenne podle jejiho id
int SymTabSearch (tBSTNodePtr RootPtr, char K, tData *Obsah)	{

	if (RootPtr == NULL) return FALSE; //neuspesne hledani
	else if (K < RootPtr->Key)
		return BSTSearch(RootPtr->LPtr, K, Content); //rekurzivni volani, hledam v levem podstromu
	else if (K > RootPtr->Key)
		return BSTSearch(RootPtr->RPtr, K, Content); //rekurzivni volani, hledam v pravem podstromu
	else{ //K=RootPtr->Key, uspesne hledani
		*Obsah = RootPtr->BSTNodeCont->Data; //vracim data o promenne
		return TRUE;
	}
}

//vlozeni noveho symbolu do tabulky
void SymTabInsert (tBSTNodePtr* RootPtr, char K, tData Obsah){
	if (*RootPtr == NULL){ //pokud nebylo alokovane, alokuju
		if ((*RootPtr = malloc (sizeof(struct tBSTNode))) == NULL) return; //zkontruluji jestli nedošlo k chybe
		(*RootPtr)->Key = K;
		(*RootPtr)->Data = Obsah;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL; //přidám položku
	}
	else if (K < (*RootPtr)->Key)
	    	BSTInsert (&(*RootPtr)->LPtr, K, Obsah);
	else if (K > (*RootPtr)->Key)
	    	BSTInsert (&(*RootPtr)->RPtr, K, Obsah);
	else (*RootPtr)->Data = Obsah; //strom toto id uz obsahuje -> prepisu obsah
}
