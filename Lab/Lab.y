%locations

%{
#include <stdio.h>
  #include <assert.h>
  #include <stdlib.h>

  #include "lexer.h"
  #include "ast.h"
 static void printNonTerminal(const char * tokName);
  int yylex (void);
  void yyerror (char const * s);

  static void printTerminal(const char * tokName);
  static void printNonTerminal(const char * tokName);
%}

%union{
  struct ast *a;
  int num;
  char *s;
}

%token <num> NUMBER 
%token <s> IDENT
%token PBEGIN PEND VAR
%token BOOL DEC
%token FOR TO DO
%token ASGN
%token ADD SUB MUL DIV LS GT EQ
%token AND OR XOR NOT
%token OP CP
%token EOL
%token ERROR

%right ASSGN
%left LS GT EQ
%left ADD SUB
%left MUL DIV
%left OR
%left AND 
%left XOR
%precedence NOT

%type <a> expr statement calclist calculations input for_statement assign_expr single_statement

%start program

%%

program: 
    input { eval($1); treefree($1); }
;

empty_line: EOL empty_line 
|%empty
;

input: empty_line declaration calculations'.' empty_line { $$ = newast('R', $3, NULL);}
;

calculations: PBEGIN EOL calclist PEND EOL { $$ = $3; }
;

declaration: VAR IDENT bool_varlist BOOL EOL declaration { createsym($2, LOG); }
| VAR IDENT dec_varlist DEC EOL declaration { createsym($2, ART); }
| %empty 
;

bool_varlist: ',' IDENT bool_varlist { createsym($2, LOG); }
| %empty
;

dec_varlist: ',' IDENT dec_varlist { createsym($2, ART); }
| %empty 
;

calclist: calclist statement { $$ = newast('L', $1, $2); }
| %empty { $$ = NULL; }
;

statement: single_statement EOL
| for_statement
| calculations
;

for_statement: FOR expr TO expr DO EOL statement { $$ = newfor($2, $4, $7); }
;

single_statement: assign_expr
| %empty { $$ = NULL; }
;

assign_expr: IDENT ASGN expr { $$ = newasgn(lookup($1), $3); }
;

expr:
  NUMBER                 { $$ = newnum($1); }
| IDENT                  { $$ = newref(lookup($1)); }
| OP expr CP                  { $$ = $2; }
| expr ADD expr                 { $$ = newop('+', ART, $1, $3); }
| expr SUB expr                 { $$ = newop('-', ART, $1, $3); }
| expr MUL expr                 { $$ = newop('*', ART, $1, $3); }
| expr DIV expr                 { $$ = newop('/', ART, $1, $3); }
| expr LS expr                 { $$ = newop('<', LOG, $1, $3); }
| expr GT expr                 { $$ = newop('>', LOG, $1, $3); }
| expr AND expr       { $$ = newop('A', LOG, $1, $3); }
| expr OR expr        { $$ = newop('O', LOG, $1, $3); }
| expr EQ expr        { $$ = newop('E', LOG, $1, $3); }
| expr XOR expr       { $$ = newop('X', LOG, $1, $3); }
| NOT expr  %prec  NOT       { $$ = newop('!', LOG, $2, NULL); }
;

%%

void yyerror(char const * msg)
{
    fprintf (stderr,
             "Ошибка: %d:%d: %s\n",
             yylloc.first_line,
             yylloc.first_column,
             msg);
    exit(1);
}


static void printTerminal(const char * tokName)
{
    printf("<%s, \"%s\", %d:%d, %d:%d>\n",
           tokName,
           yytext,
           yylloc.first_line,
           yylloc.first_column,
           yylloc.last_line,
           yylloc.last_column);
}

static void printNonTerminal(const char * tokName)
{
    printf("<%s>\n",
           tokName);
}


