# Arow

\<Type> := (int | float | bool) \<Number>? '?'?  
  
\<Function> := \<Type> \<FunctionName> '(' (\<Type> \<Variable>)?(, \<Type> \<Variable>)+ ')' (\<Stmts> | (;|<改行>))  
  
\<Stmts> := (\<Stmt> | { (\<Stmt>)+ })  
  
\<Stmt> := (  
\<Expr>(;|<改行>) |  
\<Type> \<Variable> (= \<Expr>)(;|<改行>) |  
\<Assign>(;|<改行>) |  
if '('\<Expr(bool)>')' \<Stmts> (else \<Stmts>)?  |  
while '('\<Expr(bool)>')' \<Stmts> |  
for '('\<Generator>')' \<Stmts> |  
match '('\<Expr>')' { (\<Pattern> '=>' \<Stmts>)+ }  
return \<Expr>(;|<改行>)  
)  
  
\<Assign> := \<Variable> = \<Expr>  
\<Pattern> := \<Expr> | (is | as) \<Type>  
\<generator> := \<Variable> : \<Expr(int)> to \<Expr(int)>  
  
\<Expr> := (  
\<Additive> |  
\<Additive(int)> ( < | > | <= | >= | == | != ) \<Additive(int)>  
)  
  
\<Additive> := (  
\<Multiplicative> |  
\<Multiplicative(int)> + \<Multiplicative(int)> |  
\<Multiplicative(int)> - \<Multiplicative(int)> |  
\<Multiplicative(bool)> || \<Multiplicative(bool)>  
)  
  
\<Multiplicative> := (  
\<Cast> |  
\<Cast(int)> * \<Cast(int)> |  
\<Cast(int)> / \<Cast(int)> |  
\<Cast(bool)> && \<Cast(bool)>  
)  
  
\<Cast> := \<Postfix> (as \<Type> | is \<Type> | ?)?  
  
\<Postfix> := (  
\<PrimaryExpr> |  
\<FunctionName>'('(\<Assign>)*')'  
)  
  
\<PrimaryExpr> := (  
\<Number> |  
true | false |  
None |  
".*" |  
\<Variable> |  
(\<Assign>)  
)  
  
  
## コンパイル
/bin/dcc [file] [option], ...  
  
option  
-o : filename  
