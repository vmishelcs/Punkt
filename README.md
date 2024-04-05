# Punkt

Punkt (pronounced p&upsilon;&eta;kt) means dot, point or period in German.

## Tokens

|Token type|REGEX|
|----------|-----|
| Keywords | `const` `main` `print` |
| Identifier |[ _  a-z  A-Z ] [ _ a-z A-Z 0-9 ]<sup>*</sup> |
| Boolean literal | `true` `false` |
| Integer literal | [ 0-9 ]<sup>+</sup> |
| String literal | **\"**[ ^ \" \n ]<sup>*</sup>**\"** |
| Arithmetic operators | `+` `-` `*` `/` |
| Punctuators | `{` `}` `(` `)` `=` `.` `,` |

### Notes

* Comments start with `#` and continue until either the next `#` or the end of the line.
