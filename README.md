# Punkt

Punkt (pronounced p&upsilon;&eta;kt) means dot, point or period in German.

## Tokens

|Token type|REGEX|
|----------|-----|
| *keyword* | `bool` `call` `char` `const` `else` `for` `function` `if` `int` `main` `print` `return` `string` `var` `void` |
| *identifier* |[ `_` `a`-`z`  `A`-`Z` ] [ `_` `a`-`z` `A`-`Z` `0`-`9` ]<sup>*</sup> |
| *booleanLiteral* | `true` `false` |
| *characterLiteral* | **\'** \<printable-char\> **\'** | 
| *integerLiteral* | [ `0`-`9` ]<sup>+</sup> |
| *stringLiteral* | **\"**[ ^ `"` `\n` ]<sup>*</sup>**\"** |
| Arithmetic operators | `+` `-` `*` `/` |
| Comparison operators | `==` `!=` `>` `>=` `<` `<=` |
| Punctuators | `{` `}` `(` `)` `=` `.` `,` `<` `>` `->` |

## Grammar

*Note: work in progress*

<!-- TODO: Finish this -->

```math
\begin{aligned}
Program \rightarrow & \enspace FunctionDefinition^* \enspace \texttt{main} \enspace CodeBlock \\[16pt]
FunctionDefinition \rightarrow & \enspace \texttt{function} \enspace identifier \enspace Lambda \\[16pt]
Lambda \rightarrow & \enspace \texttt{<} \enspace ParameterList \enspace \texttt{>} \enspace \texttt{->} \enspace Type \enspace CodeBlock \\[16pt]
ParameterList \rightarrow & \enspace (Type \enspace identifier \enspace (\texttt{,} \enspace Type \enspace identifier)^*)^+ \\[16pt]
CodeBlock \rightarrow & \enspace \texttt{\{} \enspace Statement^* \enspace \texttt{\}} \\[16pt]
Statement \rightarrow & \enspace DeclarationStatement \\
| & \enspace ExpressionStatement \\
| & \enspace IfStatement \\
| & \enspace ForStatement \\
| & \enspace CodeBlock \\[16pt]
DeclarationStatement \rightarrow & \enspace \texttt{const} \enspace identifier \enspace \texttt{=} \enspace Expression \enspace \texttt{.} \\
| & \enspace \texttt{var} \enspace identifier \enspace \texttt{=} \enspace Expression \enspace \texttt{.} \\[16pt]
IfStatement \rightarrow & \enspace \texttt{if} \enspace Expression \enspace CodeBlock \enspace (\texttt{else} \enspace CodeBlock)^? \\[16pt]
ForStatement \rightarrow & \enspace \texttt{for} \enspace (DeclarationStatement \enspace |  \enspace Expression) \enspace \texttt{,} \enspace Expression \enspace \texttt{,} \enspace Expression \enspace CodeBlock \\[16pt]
ExpressionStatement \rightarrow & \enspace Expression \enspace \texttt{.} \\[16pt]
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
LambdaAtomic \rightarrow & \enspace Lambda \enspace LambdaInvocation^*
\end{aligned}
```

### Notes

* Comments start with `#` and continue until either the next `#` or the end of the line.
* Maximum identifier length is 32 characters.
* Non-void functions with no return statement cause a runtime error.
* Declaration statements that are part of a for-loop should not end with a terminator (`.`).
