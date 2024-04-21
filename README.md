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

### Notes

* Comments start with `#` and continue until either the next `#` or the end of the line.
* Maximum identifier length is 32 characters.
* Non-void functions with no return statement cause a runtime error.
