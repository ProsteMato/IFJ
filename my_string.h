/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief String interface
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#ifndef _MSTRING_H
#define _MSTRING_H

#include <stdlib.h>

#define DEFAULT_STR_LEN 10

/**
 * @brief      Appends a character, reallocates the string if necesary
 *
 * @param      str   Adress of the pointer to the string
 * @param      i     Pointer to the current index in string
 * @param      cap   Pointer to the capability
 * @param[in]  c     Character wich we want to append
 *
 * @return     0 in case of error, 1 in case of success
 */
int append_char (char **str, unsigned long *i, unsigned long *cap, char c);

/**
 * @brief      Initializes my string
 *
 * @return     Pointer to the inicialized string, NULL in case of error
 */
char* mystring_init();

#endif //_MSTRING_H
