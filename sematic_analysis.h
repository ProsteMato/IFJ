
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
#include "symtable.h"
#include "scanner.h"
#include "expression_parser.h"

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
 */
void set_build_in_function_param_count(char *function_id);

/**
 * @brief This function will check if the build-in function have correct param count.
 * 
 * @param function_id Name/ID of the function
 * @param param_count Count of params
 * @return Error code SEM_PARAMS_ERROR
 */
int check_build_in_function_param_count(char *function_id, int param_count);

/**
 * @brief This function check if the given function have correct param count.
 * 
 * @param root Pointer to global symtable 
 * @param function_id Name/ID of the function
 * @param param_count Count of params
 * @return Error code SEM_PARAMS_ERROR
 */
int check_function_param_count(SymTabNodePtr root, int *function_id, int param_count);

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
 * @param second_type Type of second operand
 * @return SEM_TYPE_ERROR
 */
int arithmetic_operation_check(int first_type, int second_type);

/**
 * @brief This function will check if two types are compatible for comparison operation
 * 
 * @param first_type Type of first operand
 * @param second_type Type of second operand
 * @return SEM_TYPE_ERROR
 */
int comparison_check(int first_type, int second_type);

/**
 * @brief Function will get return type from given 2 types
 * 
 * @param first_type First operand type
 * @param secod_type Second operand type
 * @return data type
 */
Data_type arithmetic_operation_return_type(int first_type, int secod_type);

/**
 * @brief Function will give type from token
 * 
 * @param token_type token type
 * @return data type
 */
Data_type get_type_from_token(int token_type);

/**
 * @brief This function will check if function was defined.
 * 
 * @param root Pointer to global symtable
 * @param function_id Name/ID of function
 * @return SEM_FUNCTION_ERROR - if defined / OK - if not defined
 */
int is_function_defined(SymTabNodePtr root, char *function_id); // doplniť parametre

/**
 * @brief This function will check if the variable is defined.
 * 
 * @param root This is pointer to global sym table.
 * @param function_data If we are in definition of function this is pointer to local sym table otherwise global sym table
 * @param function_params If we are in definition of function this is pointer to a list of parameters otherwise NULL
 * @param variable_id Name/ID of variable
 * 
 * @return Error code SEM_FUNCTION_ERROR - if is not defined / OK - is defined
 */
int is_variable_defined(SymTabNodePtr root, LocalTableNode *function_param, ParamList *function_params, char *variable_id); // doplniť parametre

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
 * @param local_table if we are in function this is pointer to a local function otherwise pointer to a global sym table
 * @return uniq id
 */
char * expr_parser_gen_uniq_id(LocalTableNode *local_table);

/**
 * @brief This function will generate uniq id for expr_parser with prefix
 * 
 * @param local_table  if we are in function this is pointer to a local function otherwise pointer to a global sym table
 * @param prefix Prefix for uniq id.
 * @return uniq id
 */
char * expr_parser_gen_uniq_id_with_prefix(LocalTableNode *local_table, char* prefix);

#endif