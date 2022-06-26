#ifndef _AST_H_
#define _AST_H_

void yyerror(char const *);

enum type {ART, LOG};

struct symbol {
    char *name;
    int value;
    enum type type;
};

struct symbol *createsym(char *sym, enum type type);

struct symbol *lookup(char*);


/* node types
 * R root
 * P operation
 * F FOR statement
 * N symbol ref
 * = assignment
 * L statement list
 * K number
 */
struct ast {
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct for_loop {
    int nodetype;
    struct ast *begin_val;
    struct ast *end_val;
    struct ast *body;
};

struct numval {
    int nodetype;
    int number;
};

struct symref {
    int nodetype;
    struct symbol *s;
};

struct symasgn {
    int nodetype;
    struct symbol *s;
    struct ast *v;
};

/* +-/*<>
 * E equal
 * A and
 * O or
 * X xor
 * ! not
 */

struct op {
    int nodetype;
    enum type type;
    int operation;
    struct ast *lval;
    struct ast *rval;
};

struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(int num);
struct ast *newfor(struct ast *begin_val, struct ast *end_val, struct ast *body);
struct ast *newop(int operation, enum type optype, struct ast *lval, struct ast *rval);

unsigned long eval(struct ast *);

void treefree(struct ast *);
#endif