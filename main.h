/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 * @file main.h
 * @brief IFJcode19 header file for main source file
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 * @author Martin Koci <xkocim05@stud.fit.vutbr.cz>
 * @author Zuzana Hradilova <xhradi16@stud.fit.vutbr.cz>
 * @author Magdalena Ondruskova <xondru16@stud.fit.vutbr.cz>
 */

#ifndef _MAIN_H
#define _MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"
#include "expression_parser.h"
#include "error.h"
#include "generator.h"
#include "sematic_analysis.h"
#include "parser.h"
#include "scanner.h"

extern SymTabNodePtr root;
extern LocalTableNode local_table;
extern ParamList *param_list;


#endif //_MAIN_H
