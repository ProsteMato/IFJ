#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief      reprezentuje pravidlo <prog>
 * 	
 * Buď bude načítaný nejaký správny KEYWORD
 * alebo pridadenie do premennej prípadne nejaký
 * expression tak nasledujúci stav bude st-list
 * Ak bude hneď na začiatku EOF tak analýza končí
 * Inak nastáva syntaktická chyba.
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int prog(Token *token);

/**
 * @brief      
 * 	Reprezentuje pravidlá 
 * 	<st-list>, <nested-st-list>, <func-nested-st-list>
 * 	rozdielny je pri nich len mód:
 * 	- st-list = hlavne telo
 * 	- nested-st-list = if-else statement
 * 	- func-nested-st-list = platí pri deklarácii funkcii
 * 	  bude platiť aj pre if - else statementy ked sme vo funkcii
 *  Bud dostane KEYWORD, identifikátor, expression tak
 *  sa dostaneme do stavu <stat>
 *  inak syntaktická chyba.
 * 
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int st_list(Token *token);

/**
 * @brief      
 * 	Reprezentuje pravidlá 
 * 	<stat>, <nested-stat>, <func-nested-stat>
 * 	rozdielny je pri nich len mód:
 * 	- st-list = hlavne telo
 * 	- nested-st-list = if-else statement
 * 	- func-nested-st-list = platí pri deklarácii funkcii
 * 	  bude platiť aj pre if - else statementy ked sme vo funkcii
 *  KEYWORD, identifikátor, expresstion - po dokončení tohoto pravidla
 *  sa vždy očakáva EOL a následne buď jeho opetovné zavolanie alebo DEDENT
 *  ktorý ukončí danú sekvenciu vo funkcii, while, if-else statemente.
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int stat(Token *token);

/**
 * @brief      kontrolovanie prametrov funkcie pri deklarácii
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int params(Token *token);

/**
 * @brief      kontrolovanie parametrov funkcie pri deklarovanii
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int params_next(Token *token);

/**
 * @brief      kontrolovanie parametrov funckcie pri jej volaní.
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int arg_params(Token *token);

/**
 * @brief      kontrolovanie parametrov funckcie pri jej volaní.
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int arg_next_params(Token *token);

/**
 * @brief      kontrolovanie typu argumentov pri volaní funkcie.
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int value(Token *token);

/**
 * @brief      Priradzovanie identifikátoru buď výsledok funkcie alebo expresstion.
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int assign(Token *token);
#endif