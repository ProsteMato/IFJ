/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 generator implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "generator.h"

void CL_init();{
	code.first = NULL;
	code.last = NULL;
}

int CL_add_line(Code *line){
	if (line == NULL){
		return INTERNAL_ERROR;
	}
	Code_line *new = (Code_line*) malloc (sizeof(Code_line));
	if (new == NULL){
		// chyba alokacie pamate
		return INTERNAL_ERROR;
	}
	new->inst = line;
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
	if (tmp->inst != NULL){
		free(tmp->inst);
	}
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
		printf("%s\n", tmp->code->inst);
		tmp = tmp->next;
	}
	CL_destroy(Code_line code.first);
}

Code* create_code(){
	char *tmp = malloc(sizeof(Code));
	if (tmp == NULL){
		return NULL;
	}
	tmp->cap = DEFAULT_STR_LEN;
	tmp->len = 0;
	tmp->inst = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (tmp == NULL){
		return NULL;
	}
	return tmp;
}

int add_code(Code *code, char *inst){
	size_t len = strlen(inst);
	for (size_t i = 0; i < len; i++){
		if (!append_char(&(code->inst), &(code->len), &(code->cap), inst[i])){
			return INTERNAL_ERROR;
		}
	}
	if (!append_char(&(code->inst), &(code->len), &(code->cap), ' ')){
		return INTERNAL_ERROR;
	}
	return OK;
}

char* int_to_str(int i){
	char *str = malloc(sizeof(char) * INT2STR_SIZE);
	if (str == NULL){
		return NULL
	}
	sprintf(str, "%d\0", i);
	return str;
}

// -------------------
// generovacie funkcie
// -------------------

int gen_less_than(char *op1, char *op2){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	// LT GF@&expr&val op1 op2
	if (!add_code(code, "LT GF@&expr&val "))
		return INTERNAL_ERROR;

	// op1 je premenna
	if (is_variable_defined(root,op1)){
		if (is_global_variable(root,op1){
			if (!add_code(code, "GL@"))
				return INTERNAL_ERROR;
		} else {
			if (!add_code(code, "LF@"))
				return INTERNAL_ERROR;
		}
		if (!add_code(code, op1))
				return INTERNAL_ERROR;
	// op1 je konst
	} else {
		if (!add_code(code, op1))
			return INTERNAL_ERROR;
	}

	// op2 je premenna
	if (is_variable_defined(root,op2)){
		if (is_global_variable(root,op2){
			if (!add_code(code, " GL@"))
				return INTERNAL_ERROR;
		} else {
			if (!add_code(code, " LF@"))
				return INTERNAL_ERROR;
		}
		if (!add_code(code, op2))
				return INTERNAL_ERROR;
	// op1 je konst
	} else {
		if (!add_code(code, op))
			return INTERNAL_ERROR;
	}
}
int gen_more_than(char *op1, char *op2){

}
int gen_equal(char *op1, char *op2){

}
int gen_equal_less(char *op1, char *op2){

}
int gen_equal_more(char *op1, char *op2){

}
int gen_not_equal(char *op1, char *op2){

}

int gen_while_label(){  // doplnit o unique labely
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (!add_code(code, "LABEL $while$\0"))
		return INTERNAL_ERROR;
	char *un_while_n = int_to_str(while_c);
	if (un_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (!add_code(code, un_while_n))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_begin(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *unq_while_n = int_to_str(while_c);
	if (un_while_n == NULL)
		return INTERNAL_ERROR;

	if (!add_code(code, "JUMPIFEQ $while_end$\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (!add_code(code, " GL@&expr_&val bool@false"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_end(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *unq_while_n = int_to_str(while_c);
	if (un_while_n == NULL)
		return INTERNAL_ERROR;

	if (!add_code(code, "JUMP $while$\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!add_code(code, "LABEL $while_end$\0"))
		return INTERNAL_ERROR;
	if (un_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (!add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	while_c++;
	return OK;
}

int gen_int2float(char *var){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (!add_code(code, "INT2FLOAT\0"))
		return INTERNAL_ERROR;
	var = var_get_adr(var);
	if (var == NULL)
		return INTERNAL_ERROR;
	if (!add_code(code, var))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_print(char *symb){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "WRITE \0"))
		return INTERNAL_ERROR;
	if (!add_code(code, symb)) // ziskat adresu kvoli hodnote
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_f_call(char *id){
	Code *code = create_code();
	// LABEL $id
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CALL $\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, id))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}
int gen_assing_const_to_val(char *var, char *value, Token type){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (!add_code(code, "DEFVAR LF@\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, var))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, var))
		return INTERNAL_ERROR;
	if (!add_code(code, " "))
		return INTERNAL_ERROR;

	if(type == TK_INT){
		if (!add_code(code, "int@"))
			return INTERNAL_ERROR;
	} else if (type == TK_FLOAT){
		if (!add_code(code, "float@"))
			return INTERNAL_ERROR;
	} else if (type == TK_STRING){
		if (!add_code(code, "string@"))
			return INTERNAL_ERROR;
	} else {
		return INTERNAL_ERROR;
	}

	if (!add_code(code, value))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}
int gen_f_prep_params(){
	// TODO stack s parametrami
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *tmp;
	
	// CREATEFRAME
	if (!add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// vytvaranie parametrov v cykle
	for (int i = 1; !pop(stack); i++){ // TODO prepisat - kym nie je stack prazdny
		code = create_code();
		if (!code)
			return INTERNAL_ERROR;

		tmp = int_to_str(i);
		if (tmp == NULL){
			return INTERNAL_ERROR;
		}

		if (!add_code(code, "DEFVAR TF@%\0"))
			return INTERNAL_ERROR;
		if (!add_code(code, tmp))
			return INTERNAL_ERROR;
		if (!CL_add_line(code))
			return INTERNAL_ERROR;

		code = create_code();
		if (!add_code(code, "MOVE TF@%\0"))
			return INTERNAL_ERROR;
		if (!add_code(code, tmp))
			return INTERNAL_ERROR;
		if (!add_code(code, " "))
			return INTERNAL_ERROR;
		/////////////// pridat co sa movne do premennej parametru
		if (!CL_add_line(code))
			return INTERNAL_ERROR;

		free(tmp);
	}
	return OK;
}
int gen_f_start(char *id){
	Code *code = create_code();
	// LABEL $id
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, id))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// PUSHFRAME
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// DEFVAR LF@%retval
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}
int gen_f_end(char *id){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (!add_code(code, id))
		return INTERNAL_ERROR;
	if (!add_code(code, "_end\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_header(){
	Code *header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (!add_code(header, ".IFJcode19\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(header))
		return INTERNAL_ERROR;
	
	code = creat_code();
	if (!add_code(code, "DEFVAR GF@&expr&val"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputs(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "READ\0"))
		return INTERNAL_ERROR;

	char *var = get_var_adr(dest);
	if (!add_code(code, var))
		return INTERNAL_ERROR;

	if (!add_code(code, "string\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputi(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "READ\0"))
		return INTERNAL_ERROR;

	char *var = get_var_adr(dest);
	if (!add_code(code, var))
		return INTERNAL_ERROR;

	if (!add_code(code, "int\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputf(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "READ\0"))
		return INTERNAL_ERROR;

	char *var = get_var_adr(dest);
	if (!add_code(code, var))
		return INTERNAL_ERROR;

	if (!add_code(code, "float\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_len(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "# Built-in function len - returns lenght of string\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "STRLEN LF@$retval LF@%1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_ord(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "# Built-in function ord - returns ordinal value of char in s string on i index\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $ord\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// kontrola i -> musi byt v rozmedzi retazca inak return None
	// zikasnie dlzky retazca
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@len TF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// len < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LT LF@cond_val LF@len int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// i > len-1
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "SUB LF@len LF@len int@1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "GT LF@cond_val LF@%2 LF@len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "STRI2INT LF@%retval LF@%1 LF@%2\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL %ord_end\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $ord_return_none\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@%retval nil@nil\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMP $ord_end\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;


	return OK;
}

int gen_chr(){
// i mimo 0:255 vedie na behovu chybu pri praci so stringom
// INT2CHAR ju generuje sam nie je potrebna kontrola
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "# Built-in function chr - returns char whose ASCII value is i\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $chr\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "INT2CHAR LF@%retval LF@%1 LF@%2\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_substr(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "# Built-in function substr - returns substring of string\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $substr\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@%retval string@\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	// -----vytvoreny prazdny string v retval
	
	// kontroly dlzky substr a dlzky str
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@len_str\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// ziskanie retval -> dlzky stringu s
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@len_str LF@%retval\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// len(s) < 0 -> prazdny substr  ?????? mozno hlasit chybu !!!!!
	/*
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LT LF@cond_val LF@len_str int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	*/

	// len(s) == 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "EQ LF@cond_val LF@len_str int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// dlzka subst - n < 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LT LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// dlzka subst - n == 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "EQ LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// index zac subsrt i > len(s)
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "GT LF@ret_cond LF@%2 LF@len_str\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// i < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LT LF@ret_cond LF@%2 int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	// ------ kontroly vstupov hotove

	// ak je n > len(s) -> n = len(s) - i -> id indexu po koniec stringu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@max_len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@max_len LF@%2\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "SUB LF@max_len LF@max_len LF@%2\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@max_cond\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "GT LF@max_cond LF@%3 LF@max_n\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_setmax LF@max_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMP %substr_concating\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $substr_setmax\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@%3 LF@max_len\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL %substr_concating\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@idx\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "MOVE LF@idx LF@%2\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// premenne pre cyklus
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@loop_cond\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "DEFVAR LF@char\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// zaciatok cyklu pridavania pismen
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $substr_loop\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "GETCHAR LF@char LF@%1 LF@idx\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "CONCAT LF@%retval LF@%retval LF@char\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "ADD LF@idx LF@idx int@1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "SUB LF@%3 LF@%3 int@1\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// ak je n uz nula koniec cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "GT LF@loop_cond LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	// skok na zac cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "JUMPIFEQ $substr_loop LF@loop_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "LABEL $substr_end\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (!add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (!CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}