#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static unsigned long count = 1;
static unsigned int line = 0;

#define NHASH 9997
struct symbol symtab[NHASH] = {0};

static unsigned
symhash(char *sym)
{
  unsigned int hash = 0;
  unsigned c;

  while(c = *sym++) hash = hash*9 ^ c;

  return hash;
}

static void print_err(char *s){
  printf("%s\n", s);
  exit(1);
}

struct symbol *
lookup(char* sym)
{
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
  int scount = NHASH;		/* how many have we looked at */

  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) { return sp; }

    if(!sp->name) {		/* new entry */
    return NULL;
  }

    if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
}
return NULL;

}

struct symbol *createsym(char *sym, enum type type){
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
  int scount = NHASH;   /* how many have we looked at */

  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) {
      char err[128] = {0};
      sprintf(err, "Повторное определение переменной %s\n", sym);
      print_err(err);
      abort();
    }

    if(!sp->name) {   /* new entry */
    sp->name = strdup(sym);
    sp->value = 0;
    sp->type = type;
    return sp;
  }

    if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
}
print_err("symbol table overflow\n");
  abort(); /* tried them all, table is full */
}


struct ast *
newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast *
newnum(int num)
{
  struct numval *a = malloc(sizeof(struct numval));
  
  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = 'K';
  a->number = num;
  return (struct ast *)a;
}

struct ast *
newref(struct symbol *s)
{
  struct symref *a = malloc(sizeof(struct symref));
  
  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = 'N';
  a->s = s;
  return (struct ast *)a;
}

struct ast *
newasgn(struct symbol *s, struct ast *v)
{
  struct symasgn *a = malloc(sizeof(struct symasgn));
  
  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;
  a->v = v;
  return (struct ast *)a;
}

struct ast *
newfor(struct ast *begin_val, struct ast *end_val, struct ast *body)
{
  struct for_loop *a = malloc(sizeof(struct for_loop));
  
  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = 'F';
  a->begin_val = begin_val;
  a->end_val = end_val;
  a->body = body;
  return (struct ast *)a;
}

struct ast *newop(int operation, enum type optype, struct ast *lval, struct ast *rval){
  struct op *a = malloc(sizeof(struct op));

  if(!a) {
    print_err("out of space");
    exit(0);
  }
  a->nodetype = 'P';
  a->operation = operation;
  a->type = optype;
  a->lval = lval;
  a->rval = rval;
  return (struct ast *)a;
}

static void
dumpast(struct ast *a, int level);

static unsigned int count_lines(struct ast *a);
static bool is_logic(struct ast* a);

// unsigned long eval(struct ast *a){
//   return 1;
// }

unsigned long eval(struct ast *a){
  if(!a){
    print_err("internal error, null eval");
    return 0;
  }
  switch(a->nodetype){
    case 'K': printf("%i: t%lu = %d\n", ++line, count, ((struct numval *)a)->number); return count++;
    case 'N': 
    if(!((struct symref *)a)->s){
      print_err("undeclared variable");
      return 0;
    }
    printf("%i: t%lu = %s\n", ++line, count, ((struct symref *)a)->s->name); 
    return count++;
    case '=': {
      struct symasgn *asgn = (struct symasgn *)a;
      if(!asgn->s){
        print_err("undeclared variable");
        return 0;
      }
      unsigned long ret_val = 0;
      if(!asgn->v || !(ret_val = eval(asgn->v))){
        return 0;
      }
      if(asgn->s->type == LOG && !is_logic(asgn->v) ||
        asgn->s->type == ART && (is_logic(asgn->v) && asgn->v->nodetype != 'K')){
        print_err("type mismatch");
      return 0;
    }
    printf("%i: %s = t%lu\n", ++line, asgn->s->name, ret_val);
    return 1;
  }
  case 'P':
  {
    struct op *operation = (struct op *)a;
    switch(operation->operation){
      case '+': case '-': case '*': case '/':
      case '<': case '>': {
        struct ast *l = operation->lval;
        struct ast *r = operation->rval;
        if(!l || !r){
          print_err("undeclared variable");
          return 0;
        }
        if((is_logic(l) && l->nodetype != 'K') || (is_logic(r) && r->nodetype != 'K')){
          print_err("type mismatch");
          return 0;
        }
        unsigned long l_num, r_num;
        if(!(l_num = eval(l)) || !(r_num = eval(r)))
          return 0;
        printf("%i: t%lu = t%lu %c t%lu\n", ++line, count, l_num, operation->operation, r_num);
        return count++;
      }
      case 'A': case 'O': case 'X':{
        struct ast *l = operation->lval;
        struct ast *r = operation->rval;
        if(!l || !r){
          print_err("undeclared variable");
          return 0;
        }
        if(!(is_logic(l)) || !is_logic(r)){
          print_err("type mismatch");
          return 0;
        }
        unsigned long l_num, r_num;
        if(!(l_num = eval(l)) || !(r_num = eval(r)))
          return 0;
        char c;
        switch(operation->operation){
          case 'A': c = '&'; break;
          case 'O': c = '|'; break;
          case 'X': c = '^'; break;
        }
        printf("%i: t%lu = t%lu %c t%lu\n", ++line, count, l_num, c, r_num);
        return count++;
      }
      case '!':{
        struct ast *l = operation->lval;
        if(!l){
          print_err("undeclared variable");
          return 0;
        }
        if(!is_logic(l)){
          print_err("type mismatch");
          return 0;
        }
        unsigned long l_num = eval(l);
        if(!l_num)
          return 0;
        printf("%i: t%lu = !t%lu\n", ++line, count, l_num);
        return count++;
      }
      case 'E':{
        struct ast *l = operation->lval;
        struct ast *r = operation->rval;
        if(!l || !r){
          print_err("undeclared variable");
          return 0;
        }
        if(l->nodetype != 'K' && r->nodetype != 'K'){
          if(is_logic(l) && !is_logic(r) || !is_logic(l) && is_logic(r)){
            print_err("type mismatch");
            return 0;
          }
        }
        else if(l->nodetype == 'K'){
          if(is_logic(r) && !is_logic(l)){
            print_err("type mismatch");
            return 0;
          }
        }
        else if(r->nodetype == 'K'){
          if(is_logic(l) && !is_logic(r)){
            print_err("type mismatch");
            return 0;
          }
        }
        unsigned long l_num = eval(l);
        unsigned long r_num = eval(r);
        if(!l_num || !r_num){
          return 0;
        }
        printf("%i: t%lu = t%lu == t%lu\n", ++line, count, l_num, r_num);
        return count++;
      }
      default:{
        print_err("unrecognised operation");
        return 0;
      }
    }
  }
  case 'F':{
    struct for_loop *loop = (struct for_loop *)a;
    unsigned long begin, end;
    unsigned int init_line;
    struct ast *beg_val = loop->begin_val;
    struct ast *end_val = loop->end_val;
    if(!beg_val && !end_val){
      print_err("undeclared variable");
    }
    if(is_logic(beg_val) && beg_val->nodetype != 'K'){
      print_err("for begin value type mismatch");
      return 0;
    }
    if(is_logic(end_val) && end_val->nodetype != 'K'){
      print_err("for end value type mismatch");
      return 0;
    }
    if(!(begin = eval(loop->begin_val)) || !(end = eval(loop->end_val))){
      return 0;
    }
    init_line = ++line;
    printf("%i: if t%lu > t%lu goto %i\n", line, begin, end, 
      2 + init_line + count_lines(loop->body) + 1);
    if(!eval(loop->body))
      return 0;
    printf("%i: t%lu = t%lu + 1\n", ++line, begin, begin);
    printf("%i: goto %i\n", ++line, init_line);
    return 1;
  }
  case 'L':{
    unsigned long l, r;
    if(a->l)
      l = eval(a->l);
    if(a->r)
      r = eval(a->r);
    return l && r;
  }
  case 'R':
  return eval(a->l);
  default:{
    print_err("bad node");
    return 0;
  }
}
}

bool is_logic(struct ast *a){
  if(!a)
    return false;
  switch(a->nodetype){
    case 'K':{
      struct numval *val = (struct numval *)a;
      if(val->number == 1 || val->number == 0) return true;
      else return false;
    }
    case 'N':{
      struct symref *val = (struct  symref *)a;
      if(val->s && val->s->type == LOG) return true;
      else return false;
    }
    case 'P':{
      struct op *val = (struct op *)a;
      if(val->type == LOG) return true;
      else return false;
    }
    default: return false;
  }
}

unsigned int count_lines(struct ast *a){
  if(!a)
    return 0;
  switch(a->nodetype){
    case 'K': case 'N': return 1;
    case '=': return 1 + count_lines(((struct symasgn *)a)->v);
    case 'P': return 1 + count_lines(((struct op *)a)->lval) 
    + count_lines(((struct op *)a)->rval);
    case 'F': return 3 + count_lines(((struct for_loop *)a)->body) 
    + count_lines(((struct for_loop *)a)->begin_val)
    + count_lines(((struct for_loop *)a)->end_val);
    case 'L': return count_lines(a->l) + count_lines(a->r);
    case 'R': return count_lines(a->l);
    default: return 0;
  }
}

void
treefree(struct ast *a)
{
  switch(a->nodetype) {

    /* two subtrees */
    case 'L':
    if(a->r)
      treefree(a->r);

    /* one subtree */
    case 'R':
    if(a->l)
      treefree(a->l);

    /* no subtree */
    case 'K': case 'N':
    break;

    case '=':
    free( ((struct symasgn *)a)->v);
    break;

    case 'F': 
    treefree( ((struct for_loop *)a)->begin_val);
    treefree( ((struct for_loop *)a)->end_val);
    treefree( ((struct for_loop *)a)->body);
    break;

    case 'P':
    treefree( ((struct op*)a)->lval);
    treefree( ((struct op*)a)->rval);
    break;

    default: printf("internal error: free bad node %c\n", a->nodetype);
  }	  

  free(a); /* always free the node itself */

}
