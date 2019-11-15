/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#include "expression_parser.h"
#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>


const char precedenceTable[tableSize][tableSize] = {
  // 0  , 1 ,  2,   3,   4,   5.   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,   16,  17
  // +    -    *    /    //   <    <=   >   >=   ==   !=   (     )   id int  float string $
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '>'}, // +  0
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '>'}, // - 1 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '>'}, // * 2
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '>'}, // / 3
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '>'}, // // 4
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // < 5
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // <= 6 
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // > 7
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // >= 8
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // == 9
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<'}, // != 10
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '#'}, // ( 11
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '>'}, // ) 12
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '>'}, // id 13 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '>'}, // int 14
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '>'}, // float 15
  { '>', '#', '#', '#', '#', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '>'}, // string 16 
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '#', '<', '<', '<', '<', '#'}, // $ 17
};