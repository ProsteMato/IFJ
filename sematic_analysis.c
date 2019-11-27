#include "sematic_analysis.h"


bool is_build_in_function(char *function_id) {
    if (strcmp(function_id, "inputs") == 0) return true;
    if (strcmp(function_id, "inputi") == 0) return true;
    if (strcmp(function_id, "inputf") == 0) return true;
    if (strcmp(function_id, "print") == 0) return true;
    if (strcmp(function_id, "len") == 0) return true;
    if (strcmp(function_id, "substr") == 0) return true;
    if (strcmp(function_id, "ord") == 0) return true;
    if (strcmp(function_id, "chr") == 0) return true;
    
}

void set_build_in_function_param_count(SymTabNodePtr root, char *function_id) {
    if (strcmp(function_id, "inputs") == 0) SetParamCount(root, function_id, 0);
    if (strcmp(function_id, "inputi") == 0) SetParamCount(root, function_id, 0);
    if (strcmp(function_id, "inputf") == 0) SetParamCount(root, function_id, 0);
    if (strcmp(function_id, "len") == 0) SetParamCount(root, function_id, 1);
    if (strcmp(function_id, "substr") == 0) SetParamCount(root, function_id, 3);
    if (strcmp(function_id, "ord") == 0) SetParamCount(root, function_id, 2);
    if (strcmp(function_id, "chr") == 0) SetParamCount(root, function_id, 1);
}

int check_function_param_count(SymTabNodePtr root, int *function_id, int param_count) {
    GlobalTableData global_data;
    if (GlobalSymTabSearch(root, function_id, &global_data)) {
        if (global_data.pocet_par == param_count) {
            return OK;
        } else {
            return SEM_PARAMS_ERROR;
        }
    }
    return SEM_FUNCTION_ERROR;
}

Data_type build_in_function_return_type(char *function_id) {
    if (strcmp(function_id, "inputs") == 0) return TYPE_STRING;
    if (strcmp(function_id, "inputi") == 0) return TYPE_INT;
    if (strcmp(function_id, "inputf") == 0) return TYPE_FLOAT;
    if (strcmp(function_id, "print") == 0) return TYPE_NONE;
    // TODO odtialto nižšie sa kontroluje veľkosť vstupných parametrov a podľa toho sa určí navratová hodnota
    // TODO toto dole asi bude treba zmazat neviem...
    if (strcmp(function_id, "len") == 0) return TYPE_INT;
    if (strcmp(function_id, "substr") == 0) return TYPE_STRING;
    if (strcmp(function_id, "ord") == 0) return TYPE_STRING;
    if (strcmp(function_id, "chr") == 0) return TYPE_STRING;
    return TYPE_UNDEFINED;
}

int arithmetic_operation_check(Data_type first_type, pTable operation, Data_type second_type) {   
    switch(operation) {
        case PR_EDIVDIVE:
            if(first_type == TYPE_INT || second_type == TYPE_INT) {
                return OK;
            } else {
                return SEM_TYPE_ERROR;
            }
            break;
        case PR_EPLUSE:
            if(first_type == TYPE_STRING && first_type == TYPE_STRING) {
                return OK;
            }
        case PR_EDIVE:
        case PR_EMULTE:
        case PR_EMINUSE:
            if((first_type == TYPE_INT || first_type == TYPE_FLOAT) && (first_type == TYPE_INT || first_type == TYPE_FLOAT)) {
                return OK;
            }
            return SEM_TYPE_ERROR;
    }
}

int comparison_check(Data_type first_type, pTable relation, Data_type second_type) {
    switch(relation) {
        case PR_EEQE:
        case PR_ENOTEQE:
            return OK;
        case PR_EGREATE:
        case PR_EGREATEQE:
        case PR_ELESSE:
        case PR_ELESSEQE:
            if (first_type == TYPE_STRING && second_type == TYPE_STRING) {
                return OK;
            } else if ((first_type == TYPE_INT || first_type == TYPE_FLOAT) && (second_type == TYPE_INT || second_type == TYPE_FLOAT)) {
                return OK;
            }
        return SEM_TYPE_ERROR;
    }
}

Data_type arithmetic_operation_return_type(Data_type first_type, Data_type second_type) {
    if (first_type == TYPE_FLOAT && second_type == TYPE_FLOAT)
        return TYPE_FLOAT;
    if (first_type == TYPE_STRING && second_type == TYPE_STRING)
        return TYPE_STRING;
    if (first_type == TYPE_FLOAT && second_type == TYPE_INT)
        return TYPE_FLOAT;
    if (first_type == TYPE_INT && second_type == TYPE_FLOAT)
        return TYPE_FLOAT;
    if (first_type == TYPE_INT && second_type == TYPE_INT)
        return TYPE_INT;
    return TYPE_UNDEFINED;
}

Data_type get_type_from_token(SymTabNodePtr root, LocalTableNode *local_table, ParamList *params, Token token) {
    
    LocalTableData *local_data;
    GlobalTableData *global_data;
    switch(token.type) {
        case TK_FLOAT:
            return TYPE_FLOAT;
        case TK_INT:
            return TYPE_INT;
        case TK_STRING:
            return TYPE_STRING;
        case TK_KW:
            if(token.attribute == "None")
                return TYPE_NONE;
        default:
            return TYPE_UNDEFINED;
        case TK_ID:
            if (LocalSymTabSearch(*local_table, token.attribute, &local_data))
                return local_data->type;
            if (GlobalSymTabSearch(root, token.attribute, &global_data))
                return global_data->type;
    }
    return TYPE_UNDEFINED;
}

int is_function_defined(SymTabNodePtr root, char *function_id) {
    GlobalTableData *global_data;
    if (GlobalSymTabSearch(root, function_id, &global_data))
        return true;
    return false;
}

int is_global_variable(SymTabNodePtr root, char *id) {
    GlobalTableData global_data;
    if (GlobalSymTabSearch(root, id, &global_data)){
        if (!global_data.funkce)
            return true;
    }
    return false;
}

int is_variable_defined(SymTabNodePtr root, LocalTableNode *local_table, ParamList *params, char *variable_id) {
    GlobalTableData global_data;
    if (LocalSymTabSearchMinus(*local_table, variable_id))
        return true;
    if (ParamSearch(params, variable_id)) 
        return true;
    if (GlobalSymTabSearch(root, variable_id, &global_data)){
        if (!global_data.funkce)
            return true;
    }
    return false;
}

int define_function(SymTabNodePtr *root, char *function_id) {
    if (is_function_defined(*root, function_id)) {
        return SEM_FUNCTION_ERROR;
    } else {
        GlobalTableData data;
        data.funkce = true;
        GlobalSymTabInsert(root, function_id, &data);
    }
}

char * expr_parser_gen_uniq_id(SymTabNodePtr root, LocalTableNode local_table) {
    
    static int id = 1;
    char *name = "var";
    char *uniq_id = NULL;
    do {
        char *num = int_to_string(id);
        *uniq_id = (char *) malloc(sizeof(char) * strlen(name) + strlen(num) + 1);
        if (uniq_id == NULL) {
            return INTERNAL_ERROR;
        }

        strcpy(uniq_id, name);
        strncat(uniq_id, num, strlen(num));
        id++;
    } while(!is_variable_defined(root, local_table, NULL, uniq_id));

    return uniq_id;
    
}

char * expr_parser_gen_uniq_id_with_prefix(SymTabNodePtr root, LocalTableNode local_table, char* prefix) {
    static int id = 1;
    char *uniq_id = NULL;
    do {
        char *num = int_to_string(id);
        *uniq_id = (char *) malloc(sizeof(char) * strlen(num) + strlen(prefix) + 1);
        if (uniq_id == NULL) {
            return INTERNAL_ERROR;
        }

        strcpy(uniq_id, prefix);
        strncat(uniq_id, num, strlen(num));
        id++;
    } while(!is_variable_defined(root, local_table, NULL, uniq_id));

    return uniq_id;
}