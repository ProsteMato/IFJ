
/**
 * @file sematic_analysis.h
 * @author Martin Koči (xkocim05@stud.fit.vutbr.cz)
 * @brief Sematic Analysis for IFJ19
 * @date 2019-11-26
 * 
 * 
 */

#ifndef _SEM_ANALYSIS_
#define _SEM_ANALYSIS_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "expression_parser.h"
#include "scanner.h"
#include "generator.h"
/**
 * @brief This function will check if the given function is build-in function.
 * 
 * @param function_id Name/ID of the function
 * @return bool - true if it is build-in function otherwise false
 */
bool is_build_in_function(char *function_id);

/**
 * @brief This function will set build-in function param count
 * 
 * @param function_id Name/ID of the function
 * @param root Pointer to global sym table
 */
void set_build_in_function_param_count(SymTabNodePtr root, char *function_id);

/**
 * @brief This function check if the given function have correct param count.
 * 
 * @param root Pointer to global symtable 
 * @param function_id Name/ID of the function
 * @param param_count Count of params
 * @return Error code SEM_PARAMS_ERROR
 */
int check_function_param_count(SymTabNodePtr root, char *function_id, int param_count);

/**
 * @brief This function returns data type of given build-id function id
 * 
 * @param function_id Name/ID of build-in function
 * @return Data_type
 */
Data_type build_in_function_return_type(char *function_id);

/**
 * @brief This function will check if two input types are compatible for arithmetic operation
 * 
 * @param first_type Type of first operand
 * @param operation Type of operation
 * @param second_type Type of second operand
 * @return SEM_TYPE_ERROR
 */
int arithmetic_operation_check(Data_type first_type, int operation, Data_type second_type);

/**
 * @brief This function will check if two types are compatible for comparison operation
 * 
 * @param first_type Type of first operand
 * @param relation Type of relation operator
 * @param second_type Type of second operand
 * @return SEM_TYPE_ERROR
 */
int comparison_check(Data_type first_type, int relation, Data_type second_type);

/**
 * @brief Function will get return type from given 2 types
 * 
 * @param first_type First operand type
 * @param secod_type Second operand type
 * @return data type
 */
Data_type arithmetic_operation_return_type(Data_type first_type, Data_type secod_type);

/**
 * @brief Function will give type from token
 * 
 * 
 * @param token_type token type
 * @param root pointer to a global sym table
 * @param local_table pointer to a actual local table.
 * @return data type
 */
Data_type get_type_from_token(SymTabNodePtr root, LocalTableNode local_table, Token token);

/**
 * @brief This function will check if function was defined.
 * 
 * @param root Pointer to global symtable
 * @param function_id Name/ID of function
 * @return SEM_FUNCTION_ERROR - if defined / OK - if not defined
 */
int is_function_defined(SymTabNodePtr root, char *function_id); // doplniť parametre

/**
 * @brief 
 * 
 * @param root This is pointer to a global sym table
 * @param id Name/id of the global variable
 * @return Bool if is global variable true otherwise false 
 */
int is_global_variable(SymTabNodePtr root, char *id);

/**
 * @brief This function will check if the variable is defined if variable is not defined in local table of function or params
 *        it will find global table and its local table and try find there.
 * 
 * @param root This is pointer to a global sym table 
 * @param local_table If we are in definition of function this is local table of this function otherwise NULL
 * @param params if is the function this will be pointer to a params of functiou otherwise NULL
 * @param variable_id Name/ID of variable
 * 
 * @return Error code SEM_FUNCTION_ERROR - if is not defined / OK - is defined
 */
int is_variable_defined(SymTabNodePtr root,  LocalTableNode local_table, ParamList *params, char *variable_id); // doplniť parametre

/**
 * @brief This function will define function in global sym table
 * 
 * @param root Pointer to globalsym table
 * @param function_id Name/ID of function
 * 
 * @return Error code
 */
int define_function(SymTabNodePtr *root, char *function_id);

/**
 * @brief This function will generate uniq id for expr_parser
 * 
 * @param root pointer to a global sym table
 * @param local_table if we are in function this is pointer to a local function otherwise pointer to a global sym table
 * @return uniq id
 */
char * expr_parser_gen_uniq_id(SymTabNodePtr root, LocalTableNode local_table);

/**
 * @brief This function will generate uniq id for expr_parser with prefix
 * 
 * @param root  Pointer to a global sym table.
 * @param local_table Pointer to a local table of the function.
 * @param function_id If we are in definition of function this is Name/ID of this function otherwise NULL
 * @param prefix Prefix for uniq id.
 * @return uniq id
 */
char * expr_parser_gen_uniq_id_with_prefix(SymTabNodePtr root, LocalTableNode local_table, char* prefix);

#endif