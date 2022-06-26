%option nounput
%option noinput
%option noyywrap

%{
#include"grammar.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "ast.h"


/**
 * Контейнер для положения символа в тексте
 */
typedef struct
{
    int line;   /* Строка */
    int colomn; /* Столбец */
} Position_t;

/**
 *  Переменная, в которой храним текущую позицию
 */
static Position_t Position = {1, 1};

/**
 *  Сдвигаемся на #num строк
 */
static void incrLine(int num) {Position.line += num;}

/**
 * Получаем текущую строку в читаемом файле
 *
 *  Текущая строка в читаемом файле
 */
static int line(void) {return Position.line;}

/**
 *  Сдвигаемся на #num столбцов
 */
static void incrColomn(int num) {Position.colomn += num;}

/**
 *  Получаем текущий столбец в читаемом файле
 *
 *  Текущий столбец в читаемом файле
 */
static int colomn(void) {return Position.colomn;}

/**
 *  Сбрасываем текущий столбец в читаемом файле в начало строки
 */
static void dropColomn(void) {Position.colomn = 1;}

/**
 *  Автоматическое действие при чтении лексемы
 */
#define YY_USER_ACTION { \
    yylloc.first_line   = line(); \
    yylloc.last_line    = line(); \
    yylloc.first_column = colomn(); \
    yylloc.last_column  = colomn() + yyleng - 1; \
    incrColomn(yyleng); }
%}

%x COMMENT

INTEGER [0-9]+
SPACE [ \t]*
IDENTIFIER [a-zA-Z_]+
NOT_AVIABLE [^a-zA-Z_0-9+\-*/><!;,=(){}\[\]&|'" \n\t]

%%

 /* Обработка комментариев */
"{ "                    { /* Включаем состояние анализа комментариев */ BEGIN(COMMENT); }
<COMMENT>.              { /* Внутри комментария ничего не делаем */ }
<COMMENT>" }"           { /* Возвращаемся в начальное состояние */ BEGIN(INITIAL); }

"Begin" {return PBEGIN;}
"End" {return PEND;}
"Var" {return VAR;}
"+"	{return ADD; }
"-"	{ return SUB; }
"*"	{ return MUL; }
"/"	{ return DIV; }
"<" {return LS;}
">" {return GT;}
"=="    {return EQ;}
".NOT." {return NOT;}
".AND." {return AND;}
".OR."  {return OR;}
".XOR." {return XOR;}
"FOR"   {return FOR;}
"TO"    {return TO;}
"DO"    {return DO;}
":Boolean;" {return BOOL;}
":Decimal;" {return DEC;}
":="    {return ASGN;}
"(" {return OP;}
")" {return CP;}
[.,] {return yytext[0];}

{INTEGER}               { yylval.num = atoi(yytext); return  NUMBER; }
{IDENTIFIER}            { yylval.s = strdup(yytext); return  IDENT; }
{SPACE}                 { /* Пробелы игнорируем*/ }

<*>\n                   { dropColomn(); incrLine(1); yylloc.first_line++; return EOL;}

{NOT_AVIABLE}+          { return  ERROR; }

.                       { assert(!"Сюда не попадаем!"); return  ERROR; }
%%

