/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 generator implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "generator.h"

void CL_init();{
	code.first->inst = NULL;
	code.first->adr0 = NULL;
	code.first->adr1 = NULL;
	code.first->adr2 = NULL;
	code.first->next = NULL;

	code.last->inst = NULL;
	code.last->adr0 = NULL;
	code.last->adr1 = NULL;
	code.last->adr2 = NULL;
	code.last->next = NULL;
}

int CL_add_inst(char *inst,	char *adr0,	char *adr1,	char *adr2){
	if (inst == NULL){
		return INTERNAL_ERROR;
	}
	Code_line *new = (Code_line*) malloc (sizeof(Code_line));
	if (new == NULL){
		// chyba alokacie pamate
		return INTERNAL_ERROR;
	}
	new->inst = inst;
	new->adr0 = adr0;
	new->adr1 = adr1;
	new->adr2 = adr2;
	new->next = NULL;
	if (code.first == NULL && code.last == NULL){
		// ziaden prvok v liste
		code.first = new;
		code.last = new;
	} else{
		code.last->next = new;
		code.last = new;
	}
	return OK;
}

void CL_destroy(Code_line *line){
	if (line == NULL){
		return;
	}
	Code_line *tmp;
	tmp = line;
	if (tmp->inst != NULL)
		free(tmp->inst);
	if (tmp->adr0 != NULL)
		free(tmp->adr0);
	if (tmp->adr1 != NULL)
		free(tmp->adr1);
	if (tmp->adr2 != NULL)
		free(tmp->adr2);
	tmp = tmp->next;
	free(line);
	CL_destroy(tmp);
}

int init_generator(){
	CL_init();
	return gen_header();
}
void print_final_code(){
	Code_line *tmp = code.first;
	while (tmp != NULL){
		printf("%s", tmp->inst);
		if (tmp->adr0 != NULL)
			printf(" %s ", tmp->adr0);
		if (tmp->adr1 != NULL)
			printf("%s ", tmp->adr1);
		if (tmp->adr2 != NULL)
			printf("%s", tmp->adr2);
		printf("\n");
		tmp = tmp->next;
	}
	CL_destroy(Code_line code.first);
}

// -------------------
// generovacie funkcie
// -------------------

int gen_header(){
	char *header = malloc(sizeof(char) * HEADER_SIZE);
	if (header == NULL){
		return INTERNAL_ERROR;
	}
	strcpy(header, ".IFJcode19\0");
	if (ADD_INST0(header) != OK){
		return INTERNAL_ERROR;
	}
}