/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief String implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "my_string.h"

char* string_init (){
	char* str = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (str == NULL){
		// chyba alokacie pamate
		return NULL;
	}
	return str;
}

int append_char (char *str, unsigned long *i, unsigned long *cap, char c){
	if (*i >= *cap -1){
		*cap += DEFAULT_STR_LEN;
		char *tmp = realloc (str, sizeof(char) * (*cap));
		if (tmp == NULL){
			// chyba alokacie pamate
			return 0;
		} else {
			str = tmp;
		}
	}
	str[*i] = c;
	(*i)++;
	return 1;
}
