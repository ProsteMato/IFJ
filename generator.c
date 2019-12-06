/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 generator implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "generator.h"

void pq_init(){
	pq.first = NULL;
	pq.last = NULL;
}

int pq_queue(Token *token, int ret_val){
	QToken *qtk = (QToken*) malloc(sizeof(QToken));
	if (qtk == NULL){
		// internal ERROR
		return INTERNAL_ERROR;
	}
	qtk->token.attribute = token->attribute;
	qtk->token.type = token->type;
	qtk->ret_val = ret_val;
	qtk->behind = NULL;
	// no token in queue
	if (pq.last == NULL && pq.first == NULL){
		pq.last = qtk;
		pq.first = qtk;
	} else {
		pq.last->behind = qtk;
		pq.last = qtk;
	}
	return OK;
}

int pq_dequeue(Token* token){
	token->attribute = pq.first->token.attribute;
	token->type = pq.first->token.type;
	int ret_val = pq.first->ret_val;
	QToken *tmp = pq.first;
	// jeden element v queue
	if (pq.first == pq.last){
		pq.first = NULL;
		pq.last = NULL;
	} else {
		pq.first = pq.first->behind;
	}
	free(tmp);
	return ret_val;
}

int pq_first(Token *token){
	token->attribute = pq.first->token.attribute;
	token->type = pq.first->token.type;
	return pq.first->ret_val;
}

void pq_destroy(){
	QToken *tmp;
	while (pq.first != NULL){
		tmp = pq.first->behind;
		free(pq.first);
		pq.first = tmp;
	}
	pq.last = NULL;
}

void CL_init(Code_list *code_list){
	code_list->first = NULL;
	code_list->last = NULL;
}

int CL_add_line(Code_list *code_list, Code *line){
	if (line == NULL){
		return INTERNAL_ERROR;
	}
	if (!append_char(&(line->inst), &(line->len), &(line->cap), '\0')){
		return INTERNAL_ERROR;
	}
	Code_line *new = (Code_line*) malloc (sizeof(Code_line));
	if (new == NULL){
		// chyba alokacie pamate
		return INTERNAL_ERROR;
	}
	new->code = line;
	new->next = NULL;
	if (code_list->first == NULL && code_list->last == NULL){
		// ziaden prvok v liste
		code_list->first = new;
		code_list->last = new;
	} else{
		code_list->last->next = new;
		code_list->last = new;
	}
	return OK;
}

void CL_destroy(Code_line *line){
	if (line == NULL){
		return;
	}
	Code_line *tmp;
	tmp = line;
	if (tmp->code != NULL){
		free(tmp->code);
	}
	tmp = tmp->next;
	free(line);
	CL_destroy(tmp);
}

int init_generator(){
	while_counter = 0;
	if_counter = 0;
	print_counter = 0;
	s_init(&while_stack);
	s_init(&if_stack);
	CL_init(&code_list);

	CL_init(&builtin_list);
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#   BUILT-IN FUNCTIONS\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	if (gen_header()){
		return INTERNAL_ERROR;
	}
	return OK;
}

void print_final_code(){
	Code_line *tmp = code_list.first;
	while (tmp != NULL){
		printf("%s\n", tmp->code->inst);
		tmp = tmp->next;
	}
	CL_destroy(code_list.first);

	tmp = builtin_list.first;
	while (tmp != NULL){
		printf("%s\n", tmp->code->inst);
		tmp = tmp->next;
	}
	CL_destroy(builtin_list.first);

	pq_destroy();
	s_destroy(&while_stack);
	s_destroy(&if_stack);
}

Code* create_code(){
	Code *tmp = malloc(sizeof(Code));
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
	return OK;
}

char* int_to_str(int i){
	char *str = malloc(sizeof(char) * INT2STR_SIZE);
	if (str == NULL){
		return NULL;
	}
	sprintf(str, "%d", i);
	return str;
}

char* float_to_str(char *f){
	char *tmp = malloc(sizeof(char)*FLOAT2STR_SIZE);
	if (tmp == NULL){
		return NULL;
	}
	sprintf(tmp, "%a", atof(f));
	return tmp;
}

char* transform_for_write(char *str){
	Code *code = create_code();
	if (code == NULL){
		return NULL;
	}
	size_t len = strlen(str);
	for (int i = 0; i < len; i++){
		if ((str[i] >= 0 && str[i] <= 32) ||
			str[i] == 35 || str[i] == 92){
			if (!append_char(&(code->inst), &(code->len), &(code->cap), '\\'))
				return NULL;
			if (!append_char(&(code->inst), &(code->len), &(code->cap), '0'))
				return NULL;
			int jednotky = str[i]%10;
			int desiatky = (str[i] - jednotky)/10; 
			if (!append_char(&(code->inst), &(code->len), &(code->cap), desiatky+'0'))
				return NULL;
			if (!append_char(&(code->inst), &(code->len), &(code->cap), jednotky+'0'))
				return NULL;
		} else {
			if (!append_char(&(code->inst), &(code->len), &(code->cap), str[i]))
				return NULL;
		}
	}
	char *ret = code->inst;
	free(code);
	return ret;
}

// -------------------
// generovacie funkcie
// -------------------
int gen_if(){
	char *tmp = int_to_str(if_counter);
	if (tmp == NULL)
		return INTERNAL_ERROR;

	static int if_used = 0;
	if (!if_used){
		if (gen_if_exprval_check())
			return INTERNAL_ERROR;
		if_used = 1;
	}

	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL %exprval_check%\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// samotny if
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %if%\0"))
		return INTERNAL_ERROR;
	if (add_code(code, tmp))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ %false%\0"))
		return INTERNAL_ERROR;
	if (add_code(code, tmp))
		return INTERNAL_ERROR;
	if (add_code(code, " GF@&expr&val bool@true"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	free(tmp);
	s_push(&if_stack, if_counter);
	if_counter++;
	return OK;
}

int gen_else(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP %if_end%\0"))
		return INTERNAL_ERROR;

	int if_c = s_top(&if_stack);
	char *tmp = int_to_str(if_c);
	if (tmp == NULL)
		return INTERNAL_ERROR;
	if (add_code(code, tmp))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %false%\0"))
		return INTERNAL_ERROR;
	if (add_code(code, tmp))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	free(tmp);
	return OK;
}

int gen_if_end(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	int if_c = s_pop(&if_stack);
	char *tmp = int_to_str(if_c);
	if (tmp == NULL)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %if_end%\0"))
		return INTERNAL_ERROR;
	if (add_code(code, tmp))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	free(tmp);
	return OK;
}

int gen_defvar(char *var){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR \0"))
		return INTERNAL_ERROR;
	if (is_global_variable(root, var)){
		if (add_code(code, "GF@\0"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@\0"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_expr(){
	int i = 0;
	item operand = operandList.first;
	Code *code;
	char *tmp;
	while (precedenceRules[i] != -1){ // kym neprejdem cely zoznam
		if (precedenceRules[i] == PR_OPERAND ||
		    precedenceRules[i] == PR_INT ||
		  	precedenceRules[i] == PR_FLOAT || 
		  	precedenceRules[i] == PR_STRING ||
		  	precedenceRules[i] == PR_NONE){ // hodnota na stack

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS \0"))
				return INTERNAL_ERROR;

			// typ operandu
			if (operand->symbol == PT_INT){
				if (add_code(code, "int@\0"))
					return INTERNAL_ERROR;
				if (add_code(code, operand->attribute))
					return INTERNAL_ERROR;
			} else if (operand->symbol == PT_FLOAT){
				if (add_code(code, "float@\0"))
					return INTERNAL_ERROR;
				tmp = float_to_str(operand->attribute);
				if (tmp == NULL)
					return INTERNAL_ERROR;
				if (add_code(code, tmp))
					return INTERNAL_ERROR;
				free(tmp);
			} else if (operand->symbol == PT_STRING){
				if (add_code(code, "string@\0"))
					return INTERNAL_ERROR;
				tmp = transform_for_write(operand->attribute);
				if (tmp == NULL)
					return INTERNAL_ERROR;
				if (add_code(code, tmp))
					return INTERNAL_ERROR;
				free(tmp);
			} else if (operand->symbol == PT_ID){
				if(is_global_variable(root, operand->attribute)){
					if (add_code(code, "GF@\0"))
						return INTERNAL_ERROR;
				} else {
					if (add_code(code, "LF@\0"))
						return INTERNAL_ERROR;
				}
				if (add_code(code, operand->attribute))
					return INTERNAL_ERROR;
			} else {
				return INTERNAL_ERROR;
			}

			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;
			operand = operand->rptr;

		} else if (precedenceRules[i] == PR_EPLUSE){
			static int plus_used = 0;
			if (!plus_used){
				if (gen_stack_plus()){
					return INTERNAL_ERROR;
				}
				plus_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_plus$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EMINUSE){
			static int minus_used = 0;
			if (!minus_used){
				if (gen_stack_minus()){
					return INTERNAL_ERROR;
				}
				minus_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_minus$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;
			
		} else if (precedenceRules[i] == PR_EMULTE){
			static int mult_used = 0;
			if (!mult_used){
				if (gen_stack_mult()){
					return INTERNAL_ERROR;
				}
				mult_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EDIVE){
			static int div_used = 0;
			if (!div_used){
				if (gen_stack_div()){
					return INTERNAL_ERROR;
				}
				div_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_div$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EDIVDIVE){
			static int idiv_used = 0;
			if (!idiv_used){
				if (gen_stack_idiv()){
					return INTERNAL_ERROR;
				}
				idiv_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_idiv$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_ELESSE){
			static int less_used = 0;
			if (!less_used){
				if (gen_stack_less()){
					return INTERNAL_ERROR;
				}
				less_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_less$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_ELESSEQE){
			static int lesseq_used = 0;
			if (!lesseq_used){
				if (gen_stack_lesseq()){
					return INTERNAL_ERROR;
				}
				lesseq_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_lesseq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EGREATE){
			static int greater_used = 0;
			if (!greater_used){
				if (gen_stack_greater()){
					return INTERNAL_ERROR;
				}
				greater_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_greater$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EGREATEQE){
			static int greatereq_used = 0;
			if (!greatereq_used){
				if (gen_stack_greatereq()){
					return INTERNAL_ERROR;
				}
				greatereq_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_greatereq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_EEQE){
			static int eq_used = 0;
			if (!eq_used){
				if (gen_stack_equal()){
					return INTERNAL_ERROR;
				}
				eq_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_equal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

		} else if (precedenceRules[i] == PR_ENOTEQE){
			static int noteq_used = 0;
			if (!noteq_used){
				if (gen_stack_notequal()){
					return INTERNAL_ERROR;
				}
				noteq_used = 1;
			}
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CREATEFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_notequal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;
		}
		i++;
	}
	return OK;
}
int gen_assign_expr_res(char *dest){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPS \0"))
		return INTERNAL_ERROR;
	if (is_global_variable(root, dest)){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, dest))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_label(){  // doplnit o unique labely
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (add_code(code, "LABEL $while$\0"))
		return INTERNAL_ERROR;
	char *un_while_n = int_to_str(while_counter);
	if (un_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (add_code(code, un_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_begin(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *unq_while_n = int_to_str(while_counter);
	if (unq_while_n == NULL)
		return INTERNAL_ERROR;

	if (add_code(code, "JUMPIFEQ $while_end$\0"))
		return INTERNAL_ERROR;
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (add_code(code, " GF@&expr&val bool@false\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	s_push(&while_stack, while_counter);
	while_counter++;

	return OK;
}

int gen_while_end(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	int tmp = s_pop(&while_stack);
	char *unq_while_n = int_to_str(tmp);
	if (unq_while_n == NULL)
		return INTERNAL_ERROR;

	if (add_code(code, "JUMP $while$\0"))
		return INTERNAL_ERROR;
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (add_code(code, "LABEL $while_end$\0"))
		return INTERNAL_ERROR;
	if (unq_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_int2float(char *var){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "INT2FLOAT \0"))
		return INTERNAL_ERROR;
	int glob = is_global_variable(root, var);
	if (glob){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;
	if (glob){
		if (add_code(code, " GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, " LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_clear(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CLEARS\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_f_call(char *id){
	Code *code;
	char *tmp;
	int keyword = is_keyword(id, strlen(id));
	if (keyword == KW_PRINT){
		char *tmp2 = int_to_str(print_counter);
		if (tmp2 == NULL){
			return INTERNAL_ERROR;
		}
		code = create_code();
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "PUSHFRAME\0"))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		code = create_code();
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "DEFVAR LF@%retval\0"))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		code = create_code();
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "MOVE LF@%retval nil@nil\0"))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		for (int i = 1; i <= param_c; i++){
			tmp = int_to_str(i);
			if (tmp == NULL){
				return INTERNAL_ERROR;
			}

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE GF@&res1 LF@%\0"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $write$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp2))
				return INTERNAL_ERROR;
			if (add_code(code, "$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (add_code(code,  " GF@&res1 string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "WRITE string@None\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $write$end$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp2))
				return INTERNAL_ERROR;
			if (add_code(code, "$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $write$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp2))
				return INTERNAL_ERROR;
			if (add_code(code, "$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "WRITE LF@%\0"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $write$end$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp2))
				return INTERNAL_ERROR;
			if (add_code(code, "$"))
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			if (CL_add_line(&code_list, code))
				return INTERNAL_ERROR;

			if (i != param_c){
				code = create_code();
				if (!code)
					return INTERNAL_ERROR;
				if (add_code(code, "WRITE string@\\032"))
					return INTERNAL_ERROR;
				if (CL_add_line(&code_list, code))
					return INTERNAL_ERROR;
			}
		}
		code = create_code();
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "WRITE string@\\010"))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		code = create_code();
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "POPFRAME\0"))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		print_counter++;

	} else if (keyword == KW_CHR){
		static int chr_used = 0;
		if (!chr_used){
			if (gen_chr())
				return INTERNAL_ERROR;
			chr_used = 1;
		}
	} else if (keyword == KW_ORD){
		static int ord_used = 0;
		if (!ord_used){
			if (gen_ord())
				return INTERNAL_ERROR;
			ord_used = 1;
		}
	} else if (keyword == KW_INPUTI){
		static int inputi_used = 0;
		if (!inputi_used){
			if (gen_inputi())
				return INTERNAL_ERROR;
			inputi_used = 1;
		}
	} else if (keyword == KW_INPUTF){
		static int inputf_used = 0;
		if (!inputf_used){
			if (gen_inputf())
				return INTERNAL_ERROR;
			inputf_used = 1;
		}
	} else if (keyword == KW_INPUTS){
		static int inputs_used = 0;
		if (!inputs_used){
			if (gen_inputs())
				return INTERNAL_ERROR;
			inputs_used = 1;
		}
	} else if (keyword == KW_LEN){
		static int len_used = 0;
		if (!len_used){
			if (gen_len())
				return INTERNAL_ERROR;
			len_used = 1;
		}
	} else if (keyword == KW_SUBSTR){
		static int substr_used = 0;
		if (!substr_used){
			if (gen_substr())
				return INTERNAL_ERROR;
			substr_used = 1;
		}
	}
	if (keyword != KW_PRINT){
		code = create_code();
		// CALL $id
		if (!code)
			return INTERNAL_ERROR;
		if (add_code(code, "CALL $\0"))
			return INTERNAL_ERROR;
		if (add_code(code, id))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;
	}
	return OK;
}

int gen_f_prep_params(){ // parametre cez TKQueue, pridavane v spravnom poradi, teda 1. vlozeny ako 1., posledny ako posledny
	int empty = 0;
	param_c = 0;
	Token* token = malloc(sizeof(Token));
	if(token == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *tmp;
	
	// CREATEFRAME
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// vytvaranie parametrov v cykle
	if (pq.first == pq.last && pq.first == NULL){ // ziadne parametre
		free(token);
		token = NULL;
		empty = 1;
	} else {
		if (pq.first == pq.last){
			empty = 1;
		}
		if (pq_dequeue(token))
			return INTERNAL_ERROR;
	}
	for (int i = 1; token != NULL; i++){
		code = create_code();
		if (!code)
			return INTERNAL_ERROR;

		tmp = int_to_str(i);
		if (tmp == NULL){
			return INTERNAL_ERROR;
		}

		if (add_code(code, "DEFVAR TF@%\0"))
			return INTERNAL_ERROR;
		if (add_code(code, tmp))
			return INTERNAL_ERROR;
		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		code = create_code();
		if (add_code(code, "MOVE TF@%\0"))
			return INTERNAL_ERROR;
		if (add_code(code, tmp))
			return INTERNAL_ERROR;
		if (add_code(code, " "))
			return INTERNAL_ERROR;

		free(tmp);
		
		if (token->type == TK_ID){ // parameter je premenna
			if (is_global_variable(root,token->attribute)){ // globalna premenna
				if (add_code(code, "GF@\0"))
					return INTERNAL_ERROR;
			} else { // lokalna premenna
				if (add_code(code, "LF@\0"))
					return INTERNAL_ERROR;
			}
			if (add_code(code, token->attribute))
				return INTERNAL_ERROR;
		} else if (token->type == TK_FLOAT){ // konst float
			if (add_code(code, "float@"))
				return INTERNAL_ERROR;
			tmp = float_to_str(token->attribute);
			if (tmp == NULL)
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			free(tmp);
		} else if (token->type == TK_INT){ // konst int
			if (add_code(code, "int@"))
				return INTERNAL_ERROR;
			if (add_code(code, token->attribute))
				return INTERNAL_ERROR;
		} else if (token->type == TK_STRING){ // konst string
			if (add_code(code, "string@"))
				return INTERNAL_ERROR;
			tmp = transform_for_write(token->attribute);
			if (tmp == NULL)
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
			free(tmp);
		} else {
			return INTERNAL_ERROR;
		}

		if (CL_add_line(&code_list, code))
			return INTERNAL_ERROR;

		if (empty){ // prazdny queue
			free(token);
			token = NULL;
		} else {
			if (pq.first == pq.last){ // posledny v queue
				empty = 1;
			}
			if (pq_dequeue(token)){
				return INTERNAL_ERROR;
			}
		}
		param_c++;
	}
	return OK;
}

int gen_f_start(char *id){
	Code *code = create_code();
	// JUMP na koniec funckie, kvoli preskoceniu, generuje sa hned na mieste volania
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (add_code(code, "_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// LABEL $id
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// PUSHFRAME
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// DEFVAR LF@%retval
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	// MOVE LF@%retval nil@nil
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%retval nil@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_f_end(char *id){ // label na koniec
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (add_code(code, "_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_return(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPS LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_f_return(char* var){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE \0"))
		return INTERNAL_ERROR;

	if (is_global_variable(root, var)){
		if (add_code(code, "GF@\0"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@\0"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;
	if (add_code(code, " TF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_header(){
	Code *header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, ".IFJcode19\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, header))
		return INTERNAL_ERROR;

	header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, "DEFVAR GF@&expr&val"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, header))
		return INTERNAL_ERROR;

	header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, "DEFVAR GF@&expr&val&type\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, header))
		return INTERNAL_ERROR;

	header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, "DEFVAR GF@&res1"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, header))
		return INTERNAL_ERROR;

	header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, "CREATEFRAME"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, header))
		return INTERNAL_ERROR;

	return OK;
}

int gen_inputs(){
	Code *code;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  INPUTS\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $inputs$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputs\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ LF@%retval string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputs$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_inputi(){
	Code *code;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  INPUTI\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $inputi$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputi\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ LF@%retval int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputi$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_inputf(){
	Code *code;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  INPUTF\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $inputf$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputf\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ LF@%retval float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $inputf$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_len(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  LEN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $len$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@%retval LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ $len$fail LF@%retval string@string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "STRLEN LF@$retval LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $len$fail\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EXIT int@4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $len$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_ord(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  ORD\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $ord$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@type\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// kontrola typu prveho parametru s
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@type LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@bool\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// kontrola typu druheho parametru i
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@type LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord$return$4 LF@type strin@bool\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// kontrola i -> musi byt v rozmedzi retazca inak return None
	// zikasnie dlzky retazca
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@len TF@%retval\0")) // mame dlzku stringu
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// len < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@len int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// i > len-1
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@len LF@len int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@cond_val LF@%2 LF@len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "STRI2INT LF@%retval LF@%1 LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %ord_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord_return_none\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%retval nil@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $ord_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord$return$4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EXIT int@4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_chr(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  CHR\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $chr$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $chr\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%type\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@%type LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ $chr$return$4 LF@%type string@int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@%retval LF@%1 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $chr$return$58 LF@%retval bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@%retval LF@%1 int@255\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $chr$return$58 LF@%retval bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "INT2CHAR LF@%retval LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $chr$return$4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EXIT int@4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $chr$return$58\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EXIT int@58\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $chr$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_substr(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  SUBSTR\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $substr$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%retval string@\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;
	// -----vytvoreny prazdny string v retval
	
	// typove kontroly
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%type\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@%type LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ $substr$return$4 LF@%type string@string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@%type LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ $substr$return$4 LF@%type string@int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE LF@%type LF@%3\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ $substr$return$4 LF@%type string@int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// kontroly dlzky substr a dlzky str
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// ziskanie retval -> dlzky stringu s
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@len_str LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// len(s) < 0 -> prazdny substr

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@len_str int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// dlzka subst - n < 0 -> None
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr$return$none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// dlzka subst - n == 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EQ LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// index zac subsrt i > len(s) -> none
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@cond_val LF@%2 LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr$return$none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// index zac subsrt i = len(s) -> none
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EQ LF@cond_val LF@%2 LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr$return$none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// i < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@ret_cond LF@%2 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr$return$none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;
	// ------ kontroly vstupov hotove

	// ak je n > len(s) -> n = len(s) - i -> id indexu po koniec stringu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@max_len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@max_len LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@max_len LF@max_len LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "ADD LF@max_len LF@max_len int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@max_cond\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@max_cond LF@%3 LF@max_n\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ %substr_concating LF@max_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%3 LF@max_len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %substr_concating\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@idx\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@idx LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// premenne pre cyklus
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@loop_cond\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@char\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// zaciatok cyklu pridavania pismen
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr_loop\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GETCHAR LF@char LF@%1 LF@idx\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CONCAT LF@%retval LF@%retval LF@char\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "ADD LF@idx LF@idx int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@%3 LF@%3 int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// ak je n uz nula koniec cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@loop_cond LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// skok na zac cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_loop LF@loop_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr$return$4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EXIT int@4\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr$return$none\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@ret_val nil@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $substr_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&code_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr$end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_stack_plus(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_PLUS\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_plus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_plus$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $plus_not_same_type$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_concat$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// tu uz mozu byt len dva inty/float -> jump na add
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $plus_add$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// concat
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_concat$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CONCAT LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $plus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_two_floats$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_not_same_type$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// ak je ktorykolvek z nich neda sa vykonat operacia
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// jeden je float a druhy int, trepa zistit ktory je int a pretypovat ho na float

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $plus_int2float_op1$ LF@$type1$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy operator je int
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $plus_add$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_int2float_op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $plus_add$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_add$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "ADD LF@$op1$ LF@$op1$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $plus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_plus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;


			return OK;
}

int gen_stack_minus(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_MINUS\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_minus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABERL $expr_minus$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $minus_diff_types$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rovnake typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $minus_sub$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $minus_diff_type$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// ak je ktorykolvek z nich string neda sa vykonat operacia
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_exit_4$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// jeden je float a druhy int, trepa zistit ktory je int a pretypovat ho na float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $minus_int2float_op1$ LF@$type1$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy operator je int
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $minus_sub$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $minus_int2float_op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $minus_sub$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $minus_exit_4$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $minus_sub$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "SUB LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_minus_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_mult(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_MULT\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_mult_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $times_diff_types$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rovnake typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $times_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $times_diff_type$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// ak je ktorykolvek z nich string neda sa vykonat operacia
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_exit_4$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// jeden je float a druhy int, trepa zistit ktory je int a pretypovat ho na float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $times_int2float_op1$ LF@$type1$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy operator je int
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $times_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $times_int2float_op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $times_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $times_exit_4$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $times_mult$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "MUL LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_mult_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_div(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "# STACK_DIV\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_div_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_div$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $div_not_same$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_div$ LF@$type1$ string@float\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_div$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $div_zero$ LF@$op1$ float@0x0p+0\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DIV LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $div_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_zero$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@9\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy operator je int
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_not_same$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $div_int2float_op2$ LF@$type2$ string@float\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $div_div$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $div_div$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $div_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_div_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_idiv(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "# STACK_IDIV\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_idiv_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_idiv$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// musia byt rovnake typy
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $idiv_fail$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// musia byt int
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFNEQ $idiv_fail$ LF@$type1$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $idiv_zero$ LF@$op1$ int@0\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "IDIV LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;
					code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $idiv_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $idiv_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $idiv_zero$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@9\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $idiv_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_idiv_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_equal(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_EQUAL\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_equal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_equal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_eq$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// ake je jeden z nich nil tak sa operacia da stale vykonat
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;


			// ak je jeden z nich bool ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_false$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $equal_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $equal_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EQ LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $equal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $equal_false$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "MOVE LF@$op1$ bool@false\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $equal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $equal_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $equal_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $equal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_equal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_notequal(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "# STACK_NOTEQUAL\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_notequal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_notequal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "CALL $expr_equal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS GF@&res1\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "NOT GF@&res1 GF@&res1\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS GF@&res1\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_notequal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
			/*
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_notequal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_notequal$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_eq$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// 
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_eq$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_eq$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $notequal_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $notequal_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EQ LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $notequal_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $notequal_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $notequal_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $notequal_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $notequal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "NOT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_noteqal_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
			*/
}

int gen_stack_less(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_LESS\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_less_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_less$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_less$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $less_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $less_less$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LT LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $less_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $less_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $less_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $less_less$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $less_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_less_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_lesseq(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "# STACK_LESSEQ\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_lesseq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_lesseq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$res_less$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$res_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_lteq$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $lesseq_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $lesseq_lteq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LT LF@$res_less$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EQ LF@$res_eq$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $lesseq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $lesseq_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $lesseq_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $lesseq_lteq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $lesseq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "OR LF@$op1$ LF@$res_less$ LS@$res_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_lesseq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_greater(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_GREATER\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_greater_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_greater$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_gt$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greater_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greater_gt$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "GT LF@$op1$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $greater_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greater_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greater_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $greater_gt$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greater_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_greater_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_stack_greatereq(){
			Code *code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "#  STACK_GREATEREQ\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $expr_greatereq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_greatereq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$res_greater$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$res_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "DEFVAR LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPS LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "TYPE LF@$type2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type1$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type2$ string@bool\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type1$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type2$ string@nil\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_gteq$ LF@$type1$ LF@$type2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// rozdielne typy
			// ak je jeden z nich string ta sele zle
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type1$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_fail$ LF@$type2$ string@string\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// druhy op je int a prvy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMPIFEQ $greateq_int2float_op2$ LF@$type2$ string@int\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			// prvy op je int a druhy float
			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op1$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greateq_gteq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "GT LF@$res_greater$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EQ LF@$res_eq$ LF@$op2$ LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $greateq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greateq_fail$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "EXIT int@4\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greateq_int2float_op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "INT2FLOAT LF@$op2$ LF@$op2$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "JUMP $greateq_gteq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $greateq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "OR LF@$op1$ LF@$res_less$ LS@$res_eq$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "PUSHS LF@$op1$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "POPFRAME\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "RETURN\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			code = create_code();
			if (!code)
				return INTERNAL_ERROR;
			if (add_code(code, "LABEL $expr_greatereq_end$\0"))
				return INTERNAL_ERROR;
			if (CL_add_line(&builtin_list, code))
				return INTERNAL_ERROR;

			return OK;
}

int gen_if_exprval_check(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "#  IF_EXPRVAL_CHECK\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP %exprval_check%end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPS GF@&expr&val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "TYPE GF@&expr&val&type GF@&expr&val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// vysledok expr je bool
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFNEQ %exprval_check%not_bool% GF@&expr&val&type string@bool\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// vysledok expr neni bool
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%not_bool%\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// jump na None
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%false GF@&expr&val&type string@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// jump na int
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%int GF@&expr&val&type string@int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// jump na float
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%float GF@&expr&val&type string@float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// jump na string
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%string GF@&expr&val&type string@string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// int
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%false GF@&expr&val int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE GF@&expr&val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// float
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%false GF@&expr&val float@0x0p+0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE GF@&expr&val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// string
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ %exprval_check%false GF@&expr&val string@\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE GF@&expr&val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	// false do exprval
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%false\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE GF@&expr&val bool@false\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %exprval_check%end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(&builtin_list, code))
		return INTERNAL_ERROR;

	return OK;
}
