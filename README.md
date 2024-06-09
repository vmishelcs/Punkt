# Punkt

Punkt (pronounced p&upsilon;&eta;kt) means dot, point or period in German.

## Tokens

|Token type|REGEX|
|----------|-----|
| *keyword* | `alloc` `bool` `char` `const` `dealloc` `else` `false` `for` `function` `if` `int` `main` `print` `println` `rat` `return` `sizeof` `string` `true` `var` `void` `while` |
| *identifier* |[ _ a-z A-Z ] [ _ a-z A-Z 0-9 ]<sup>*</sup> |
| *keyword* | `alloc` `bool` `char` `const` `dealloc` `else` `false` `for` `function` `if` `int` `main` `print` `println` `rat` `return` `sizeof` `string` `true` `var` `void` `while` |
| *identifier* |[ _ a-z A-Z ] [ _ a-z A-Z 0-9 ]<sup>*</sup> |
| *booleanLiteral* | `true` `false` |
| *characterLiteral* | `'` *ASCII character* `'` | 
| *integerLiteral* | [ 0-9 ]<sup>+</sup> |
| *stringLiteral* | `"`[ ^ `"` `\n` ]<sup>*</sup>`"` |
| Arithmetic operators | `+` `-` `*` `/` |
| Other operators | `=` `%` `//` `!` `&&` `\|\|` `sizeof` `alloc` `[]`|
| Comparison operators | `==` `!=` `>` `>=` `<` `<=` |
| Punctuators | `{` `}` `(` `)` `.` `,` `<` `>` `->` |

## Grammar

This is a description of Punkt grammar, simplified slightly for better readability. The grammar that the Punkt parser actually implements is entirely unambiguous.

```math
\begin{aligned}
Program \rightarrow & \enspace FunctionDefinition^* \enspace \texttt{main} \enspace CodeBlock \\[16pt]

FunctionDefinition \rightarrow & \enspace \texttt{function} \enspace identifier \enspace Lambda \\[16pt]

Lambda \rightarrow & \enspace \texttt{<} \enspace ParameterList^? \enspace \texttt{>} \enspace \texttt{->} \enspace Type \enspace CodeBlock \\[16pt]

ParameterList \rightarrow & \enspace Type \enspace identifier \enspace (\texttt{,} \enspace Type \enspace identifier)^* \\[16pt]

CodeBlock \rightarrow & \enspace \texttt{\{} \enspace Statement^* \enspace \texttt{\}} \\[16pt]
Statement \rightarrow & \enspace CodeBlock \\
| & \enspace DeclarationStatement \\
| & \enspace ExpressionStatement \\
| & \enspace IfStatement \\
| & \enspace WhileStatement \\
| & \enspace ForStatement \\
| & \enspace ReturnStatement \\
| & \enspace PrintStatement \\[16pt]

DeclarationStatement \rightarrow & \enspace \texttt{const} \enspace identifier \enspace \texttt{=} \enspace Expression \enspace \texttt{.} \\
| & \enspace \texttt{var} \enspace identifier \enspace \texttt{=} \enspace Expression \enspace \texttt{.} \\[16pt]

ExpressionStatement \rightarrow & \enspace Expression \enspace \texttt{.} \\[16pt]

IfStatement \rightarrow & \enspace \texttt{if} \enspace Expression \enspace (Statement \enspace | \enspace CodeBlock) \enspace (\texttt{else} \enspace (Statement \enspace | \enspace CodeBlock))^? \\[16pt]

WhileStatement \rightarrow & \enspace \texttt{while} \enspace Expression \enspace (Statement \enspace | \enspace CodeBlock) \\[16pt]

ForStatement \rightarrow & \enspace \texttt{for} \enspace (DeclarationStatement \enspace |  \enspace Expression) \enspace \texttt{,} \enspace Expression \enspace \texttt{,} \enspace Expression \enspace CodeBlock \\[16pt]

ReturnStatement \rightarrow & \enspace \texttt{return} \enspace Expression^? \enspace \texttt{.} \\[16pt]

PrintStatement \rightarrow & \enspace \texttt{print} \enspace PrintExpressionList \enspace \texttt{.} \\
| & \enspace \texttt{println} \enspace PrintExpressionList \enspace \texttt{.} \\[16pt]

PrintExpressionList \rightarrow & \enspace Expression \enspace (\texttt{,} \enspace Expression)^* \\[16pt]

Type \rightarrow & \enspace BaseType \\
| & \enspace ArrayType \\
| & \enspace LambdaType \\[16pt]

BaseType \rightarrow & \enspace \texttt{void} \\
| & \enspace \texttt{bool} \\
| & \enspace \texttt{char} \\
| & \enspace \texttt{int} \\
| & \enspace \texttt{rat} \\
| & \enspace \texttt{rat} \\
| & \enspace \texttt{string} \\[16pt]

ArrayType \rightarrow & \enspace \texttt{[} \enspace Type \enspace \texttt{]} \\[16pt]

LambdaType \rightarrow & \enspace \texttt{<} \enspace TypeList^? \enspace \texttt{>} \enspace \texttt{->} \enspace Type \\[16pt]

TypeList \rightarrow & \enspace Type \enspace (\texttt{,} \enspace Type)^*

\end{aligned}
```

<!-- Not sure if this level of detailed expression grammar is necessary.

Expression \rightarrow & \enspace AssignmentExpression \\[16pt]

AssignmentExpression \rightarrow & \enspace EqualityExpression \enspace \left((\ \texttt{=}\ |\ \texttt{+=}\ |\ \texttt{-=}\ |\ \texttt{*=}\ |\ \texttt{/=}\ |\ \texttt{\%=}\ ) \enspace EqualityExpression \right)^? \\[16pt]

EqualityExpression \rightarrow & \enspace ComparisonExpression \enspace ((\ \texttt{==}\ |\ \texttt{!=}\ ) \enspace ComparisonExpression)^* \\[16pt]

ComparisonExpression \rightarrow & \enspace AdditiveExpression \enspace ((\ \texttt{>}\ |\ \texttt{>=}\ |\ \texttt{<}\ |\ \texttt{<=}\ ) \enspace AdditiveExpression)^* \\[16pt]

AdditiveExpression \rightarrow & \enspace MultiplicativeExpression \enspace ((\ \texttt{+}\ |\ \texttt{-}\ ) \enspace MultiplicativeExpression)^* \\[16pt]

MultiplicativeExpression \rightarrow & \enspace UnaryExpression \enspace ((\ \texttt{*}\ |\ \texttt{/}\ |\ \texttt{\%}\ ) \enspace UnaryExpression)^* \\[16pt]

UnaryExpression \rightarrow & \enspace Atomic \\
| & \enspace \texttt{-} \enspace UnaryExpression \\
| & \enspace \texttt{+} \enspace UnaryExpression \\[16pt]

Atomic \rightarrow & \enspace \texttt{(} \enspace Expression \enspace \texttt{)} \\
| & \enspace IdentifierAtomic \\
| & \enspace LambdaAtomic \\
| & \enspace booleanLiteral \\
| & \enspace characterLiteral \\
| & \enspace integerLiteral \\
| & \enspace stringLiteral \\[16pt]

IdentifierAtomic \rightarrow & \enspace identifier \enspace LambdaInvocation^* \\[16pt]

LambdaAtomic \rightarrow & \enspace Lambda \enspace LambdaInvocation^* \\[16pt]
-->

### Notes

* Comments start with `#` and continue until either the next `#` or the end of the line.
* Maximum identifier length is 32 characters.
* Non-void functions with no return statement cause a runtime error.
* Declaration statements that are part of a for-loop should not end with a terminator (`.`).
