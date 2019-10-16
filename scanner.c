/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Scanner implementation.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "scanner.h"

int get_next_token(FILE* source){
	// inicializacia premennych
	char* str = malloc(DEFAULT_STR_LEN);
	unsigned long str_i = 0;
	char c;
	int state = SCANST_START;

	while(TRUE){
		c = getc(source);
		switch(state){
			case (SCANST_START):
				if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_'){
					state = SCANST_START_OF_KW_ID;
				} else if (c >= '0' && C <= '9'){
					state = SCANST_NUMBER;
				} else if (isspace(c)){
					stare = SCANST_START;
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else if (){
					
				} else {

				}
				break;
			case (SCANST_EOL):
				break;
			case (SCANST_START_OF_KW_ID):
				break;
			case (SCANST_NUMBER):
				break;
			case ():
				break;
			case ():
				break;
			default:
				break;
		}
	}
}