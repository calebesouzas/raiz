# Development Progress
## Current Step

-

## Roadmap
### Lexer
Turn source code raw text (pure ASCII) into sequence of tokens.

**Syntax**:
Name 'symbol'; where it can be used (optional note)...

List of tokens.
- [ ] Plus '+'; infix operator, self update.
- [ ] Minus '-'; infix operator, unary operator, self update.
- [ ] Star '*'; locations:
    - infix: multiplication
    - unary: dereference indicator (before the variable, when not passing)
    - self update: multiplication
    - before type name: raw pointer indicator
- [ ] Slash '/'; locations:
    - infix: division
    - self update: division
    - double: single line comment (ends with a '\n' byte)
- [ ] Ampersand '&'; locations:
    - unary: reference (incrementing reference counter)
    - infix: bitwise and
    - self update: bitwise and
    - before type name: reference indicator
- [ ] Pipe '|'; infix, self update: bitwise or, separator (pass through) in 
`if-in-case`.
- [ ] Hat '^'; infix, self update: bitwise exclusive or.
- [ ] Back slash '\\'; use keyword token as an identifier.
