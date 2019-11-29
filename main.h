/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 header file for main source file
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#ifndef _MAIN_H
#define _MAIN_H

#include "symtable.h"
#include "expression_parser.h"
#include "error.h"
#include "generator.h"
#include "sematic_analysis.h"
#include "parser.h"
#include "scanner.h"

SymTabNodePtr root;
LocalTableNode local_table;
ParamList *param_list;


#endif //_MAIN_H