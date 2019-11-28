#include "../sematic_analysis.h"
#include <stdio.h>
#include "sem_test.h"


int main(int argc, char const *argv[])
{
    char *build_in[] = {"inputi", "inputf", "inputs", "ord", "chr", "substr", "print", "len"};
    char *build_in_without_print[] = {"inputi", "inputf", "inputs", "ord", "chr", "substr", "len"};
    int id[] = {0, 0, 0, 2, 1, 3, 1};
    root = malloc(sizeof(struct SymTabNode));
    GlobalSymTabInit(root);

    printf("Kontrola build in functions\n");
    for (int i = 0; i < 8; i++){
        if(!is_build_in_function(build_in[i])){
            printf("chyba v \"%s\"\n", build_in[i]);
            return 0;
        }
    }
    printf("Kontrola build in functions == OK\n");

    printf("Deklarácie build-in functions\n");
    for (int i = 0; i < 8; i++){
        if(define_function(root, build_in[i])){
            printf("chyba v \"%s\"\n", build_in[i]);
            return 0;
        }
    }
    printf("Deklarácie build-in functions == OK\n");

    printf("Kontrola deklarácie build-in functions\n");
    for (int i = 0; i < 8; i++){
        if(!is_function_defined(*root, build_in[i])){
            printf("chyba v \"%s\"\n", build_in[i]);
            return 0;
        }
    }
    printf("Kontrola Deklarácie build-in functions == OK\n");
    
    printf("Natavenie parametrov build in functions\n");
    for (int i = 0; i < 7; i++){
        set_build_in_function_param_count(*root, build_in_without_print[i]);
    }
    printf("Natavenie parametrov build in functions == OK\n");

    printf("Kontrola parametrov build in functions\n");
    for (int i = 0; i < 7; i++){
        if(check_function_param_count(*root, build_in_without_print[i], id[i]) != OK){
            printf("chyba v \"%s\"\n", build_in_without_print[i]);
            return 0;
        }
    }
    printf("Kontrola parametrov build in functions == OK\n");
    printf("kontrola aritmetických operacii\n");
    if(arithmetic_operation_check(TYPE_FLOAT, PR_EPLUSE, TYPE_FLOAT) != OK){
        printf("chyba 1\n");
        return 0;
    }
    if(arithmetic_operation_check(TYPE_INT, PR_EPLUSE, TYPE_FLOAT) != OK){
        printf("chyba 2\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_FLOAT, PR_EPLUSE, TYPE_INT) != OK){
        printf("chyba 3\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_STRING, PR_EPLUSE, TYPE_STRING) != OK){
        printf("chyba 4\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_STRING, PR_EPLUSE, TYPE_FLOAT) == OK){
        printf("chyba 5\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_FLOAT, PR_EPLUSE, TYPE_STRING) != OK){
        printf("chyba 6\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_INT, PR_EDIVDIVE, TYPE_INT) != OK){
        printf("chyba 7\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_STRING, PR_EDIVDIVE, TYPE_FLOAT) == OK){
        printf("chyba 8\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_FLOAT, PR_EDIVDIVE, TYPE_FLOAT) == OK){
        printf("chyba 9\n");
        return 0;
        }
    if(arithmetic_operation_check(TYPE_STRING, PR_EDIVE, TYPE_STRING) == OK){
        printf("chyba 10\n");
        return 0;
        }
    printf("kontrola aritmetických operacii == OK\n");
    define_global_variable(root, "var1");
    if(!is_variable_defined(*root, NULL, NULL, "var1")) {
        printf("chyba premenna ma byt definovaná\n");
        return 0;
    }
    printf("created global variable\n");
    char *variable = expr_parser_gen_uniq_id(root, NULL);
    printf("%s\n", variable);
    GlobalSymTabDispose(root);
    return 0;
}
