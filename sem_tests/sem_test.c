#include "../sematic_analysis.h"
#include <stdio.h>


int main(int argc, char const *argv[])
{
    char *build_in[] = {"inputi", "inputf", "inputs", "ord", "chr", "substr", "print", "len"};
    char *build_in_without_print[] = {"inputi", "inputf", "inputs", "ord", "chr", "substr", "len"};
    int id[] = {0, 0, 0, 2, 1, 3, 1};
    SymTabNodePtr *root;
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
    for (int i = 0; i < 8; i++){
        if(check_function_param_count(*root, build_in[i], id[i]) != OK){
            printf("chyba v \"%s\"\n", build_in[i]);
            return 0;
        }
    }
    printf("Kontrola parametrov build in functions == OK\n");
    GlobalSymTabDispose(root);
    return 0;
}
