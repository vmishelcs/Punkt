# Punkt

Punkt (pronounced p&upsilon;&eta;kt) means dot, point or period in German.

## Tokens

|Token type|REGEX|
|----------|-----|
| Keywords | `const` `else` `for` `if` `main` `print` `var` |
| Identifier |[ `_` `a`-`z`  `A`-`Z` ] [ `_` `a`-`z` `A`-`Z` `0`-`9` ]<sup>*</sup> |
| Boolean literal | `true` `false` |
| Character literal | **\'** \<printable-char\> **\'** | 
| Integer literal | [ `0`-`9` ]<sup>+</sup> |
| String literal | **\"**[ ^ `"` `\n` ]<sup>*</sup>**\"** |
| Arithmetic operators | `+` `-` `*` `/` |
| Comparison operators | `==` `!=` `>` `>=` `<` `<=` |
| Punctuators | `{` `}` `(` `)` `=` `.` `,` |

## Grammar

### Notes

* Comments start with `#` and continue until either the next `#` or the end of the line.
* Maximum identifier length is 32 characters.
