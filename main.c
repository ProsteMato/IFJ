/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJ19 to IFJcode19 compiler
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 * @author Martin Koci <xkocim05@stud.fit.vutbr.cz>
 * @author Zuzana Hradilova <xhradi16@stud.fit.vutbr.cz>
 * @author Magdalena Ondruskova <xondru16@stud.fit.vutbr.cz>
 */

#include "main.h"

int main (int argc, char * argv[]){
	// init globalnej tabulky symbolov
	GlobalSymTabInit(root);
	Token token;
	int ret_val;
	ret_val = get_next_token(&token);
	if (ret_val){
		return ret_val;
	}

	ret_val = prog(&token);
	if (ret_val != OK){
		return ret_val;
	}
	print_final_code();
	GlobalSymTabDispose(root);
	return OK;
}