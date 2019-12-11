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

SymTabNodePtr root;
LocalTableNode local_table;
ParamList *param_list;


int main (int argc, char * argv[]){
	// init globalnej tabulky symbolov
	GlobalSymTabInit(&root);
	if (init_generator()){
		return INTERNAL_ERROR;
	}
	Token token;
	int ret_val;
	ret_val = get_next_token(&token);
	if (ret_val){
		return ret_val;
	}
	char *build_in_functions[] = {"inputi", "inputf", "inputs", "ord", "chr", "substr", "print", "len"};
	for(int i = 0; i < 8; i++) {
		if((ret_val = define_function(&root, build_in_functions[i])) != OK) {
			return ret_val;
		}
		SetDefine(root, build_in_functions[i]);
		if (strcmp(build_in_functions[i], "print") != 0) {
			set_build_in_function_param_count(root, build_in_functions[i]);
		}
	}

	ret_val = prog(&token);
	if (ret_val != OK){
		return ret_val;
	}
	print_final_code();
	GlobalSymTabDispose(&root);
	return OK;
}
