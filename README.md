# Arow

## 構文
### 記法

`
int  
`  
これは文字列を表しています。  
`
'int'  
`  
これも同様です。シングルクォートで囲むのは一部の文字列に特別な意味を持たせるため、それと混同しないようにする為です。  
  
`
(int | var | '(')
`  
()はグルーピングという意味で''よりも優先します。|はまたはという意味でこの場合intかvarか(がありえます。  
  
`
\<hoge> :=
`  
  
`
\<>
`  
  
### 本体
  
\<Function> := \<Type> \<FunctionName> '(' (\<Type> \<Variable>)?(, \<Type> \<Variable>)+ ')' (\<Stmts> | (;|<改行>))  
  
\<Type> := (int | float | bool) \<Number>? '?'?  
  
\<FunctionName> := (String)  
  
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
