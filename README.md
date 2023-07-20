# Simple compiler
## Description
Simple compiler that translates program with defined grammar into the three-address code.
## Grammar
```
<Program> ::= <Variable declaration> <Calculation description>.
<Calculation description> ::= Begin <List of operators> End
<Variable declaration> ::= Var <List of variables> :Boolean;
Var <List of variables> :Boolean;| <Variable declaration>
<Variable declaration> ::= Var <List of variables> :Decimal;|
Var <List of variables> :Decimal; <Variable declaration>
<List of operators> ::= <Operator> | <Operator> <List of operators>
<Operator>::=<Assignment> |<Complex operator>
<Assignment> ::= <Id> := <Expression>
<Expression> ::= <Un.op.> <Sub-expression> | <Sub-expression>
<Sub-expression> :: = ( <Expression> ) | <Operand> |
< Sub-expression > <Bin.op.> <Sub-expression>
<Complex operator>:: =<Loop> | <Compound operator>
<Loop>:: =FOR <Id> ":=" <LoopList> DO <Operator>
<LoopList> :: = <BegVal> TO <EndVal>
<BegVal> :: = <Expression>
<EndVal>:: = <Expression>
<Compound operator> ::= “Begin” <List of operators> “End”
<Un.op.> ::= ".NOT."
<Bin.op.> ::= ".AND." | ".OR." | ".XOR." | "-" | "+" | "*" | "/" | ">" | "<" | "=="
<Operand> ::= <Id> | <Const>
<Id> ::= <Letter> <Id> | <Letter>
<Const> ::= < BConst >|< DConst > 
< DConst ><Digit> |<Digit><DConst>
< BConst > ::= 0|1
```
## Build
`make all`
## Run
`./main <path-to-program>`
## Example
### Source
```
{ comment. }
Var a,c:Decimal; 
Var b, d:Boolean;
Begin
{ two-lines.
comment. }
a := 18
c := 24 - 8 + a + 9 * 3
b := 1
d := 0
{ three 
  lines 
  comment }
b := b.OR..NOT.b.AND.d.XOR.b
FOR 8 + c TO a * 24 DO 
FOR 16-a TO 12/a DO 
Begin
c := a - c + 1
a := c -a- 1
FOR c - a TO c+a DO 
d := .NOT.d
End
b:= 1 .AND.0.OR. b
End
.
{ end 
comment }
```
### Result
```
1: t1 = 18
2: a = t1
3: t2 = 24
4: t3 = 8
5: t4 = t2 - t3
6: t5 = a
7: t6 = t4 + t5
8: t7 = 9
9: t8 = 3
10: t9 = t7 * t8
11: t10 = t6 + t9
12: c = t10
13: t11 = 1
14: b = t11
15: t12 = 0
16: d = t12
17: t13 = b
18: t14 = b
19: t15 = !t14
20: t16 = d
21: t17 = b
22: t18 = t16 ^ t17
23: t19 = t15 & t18
24: t20 = t13 | t19
25: b = t20
26: t21 = 8
27: t22 = c
28: t23 = t21 + t22
29: t24 = a
30: t25 = 24
31: t26 = t24 * t25
32: if t23 > t26 goto 68
33: t27 = 16
34: t28 = a
35: t29 = t27 - t28
36: t30 = 12
37: t31 = a
38: t32 = t30 / t31
39: if t29 > t32 goto 66
40: t33 = a
41: t34 = c
42: t35 = t33 - t34
43: t36 = 1
44: t37 = t35 + t36
45: c = t37
46: t38 = c
47: t39 = a
48: t40 = t38 - t39
49: t41 = 1
50: t42 = t40 - t41
51: a = t42
52: t43 = c
53: t44 = a
54: t45 = t43 - t44
55: t46 = c
56: t47 = a
57: t48 = t46 + t47
58: if t45 > t48 goto 64
59: t49 = d
60: t50 = !t49
61: d = t50
62: t45 = t45 + 1
63: goto 58
64: t29 = t29 + 1
65: goto 39
66: t23 = t23 + 1
67: goto 32
68: t51 = 1
69: t52 = 0
70: t53 = t51 & t52
71: t54 = b
72: t55 = t53 | t54
73: b = t55
```
[More examples](./Lab/examples)