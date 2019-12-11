/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 * @file my_string.c
 * @brief String implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "my_string.h"

char* mystring_init (){
	char* str = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (str == NULL){
		// chyba alokacie pamate
		return NULL;
	}
	return str;
}

int append_char (char **str, unsigned long *i, unsigned long *cap, char c){
	char *tmp = NULL;
	if (*i >= (*cap) -1){
		*cap += DEFAULT_STR_LEN;
		tmp = (char *) realloc (*str, sizeof(char) * (*cap));
		if (tmp == NULL){
			// chyba alokacie pamate
			return 0;
		} else {
			*str = tmp;
		}
	}
	(*str)[*i] = c;
	(*i)++;
	return 1;
}
