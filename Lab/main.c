 
#include <stdio.h>
#include "lexer.h"
#include "grammar.h"

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        fprintf(stderr, "x: ошибка: не задан входной файл\n");
        return 1;
    }

    int     res;    /* Результат работы программы */
    FILE    * f;    /* Входной файл */

    f = fopen(argv[1], "r");

    if( f == NULL )
    {
        fprintf(stderr, "x: ошибка: не удалось открыть входной файл `%s'\n",
                argv[1]);
        return 2;
    }

    yyin    = f;
    res     = yyparse();
    fclose(f);

    return res;
}
