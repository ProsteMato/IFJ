

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "../symtable.h"

// funkce pro testovani
int main(int argc, char *argv[]){
	printf("Inicialuzuju a vlozim uzel: \n");

	SymTabNodePtr novynode = malloc(sizeof(struct SymTabNode));
	GlobalTableData *nejakadata = malloc(sizeof(struct GlobalTableData));
	
	char *Key = "nejaky kliiiiiiiiiic";
	nejakadata->type = TYPE_INT;
	nejakadata->funkce = true;
	nejakadata->define = false;
	
	char *OtherKey = "nejaky jiny klic";

	GlobalTableData *jinadata = malloc(sizeof(struct GlobalTableData));
	jinadata->type = TYPE_STRING;
	jinadata->funkce = false;
	jinadata->define = true;

	GlobalTableData *misto;


	GlobalSymTabInit(&novynode);
	GlobalSymTabInsert(&novynode, Key, nejakadata);

	printf(" Klic: %s \n", novynode->Key);
	printf(" Data (datovy typ): %d \n", novynode->Data->type);
	printf(" Data (je funkce): %d \n", novynode->Data->funkce);
	printf(" Data (je definovano): %d \n \n", novynode->Data->define);

	printf("Vlozim dalsi uzel: \n");

	GlobalSymTabInsert(&novynode, OtherKey, jinadata);
	printf(" Klic: %s \n", novynode->LPtr->Key);
	printf(" Data (datovy typ): %d \n", novynode->LPtr->Data->type);
	printf(" Data (je funkce): %d \n", novynode->LPtr->Data->funkce);
	printf(" Data (je definovano): %d \n \n", novynode->LPtr->Data->define);

	printf("zkusim hledat OtherKey: %d \n", (GlobalSymTabSearch(novynode, OtherKey, &misto)));
	char * NeexistujiciKlic = "nejsem tu";
	printf("zkusim hledat něco co tam neni: %d \n", (GlobalSymTabSearch(novynode, NeexistujiciKlic, &misto)));
	
	return 0;
}