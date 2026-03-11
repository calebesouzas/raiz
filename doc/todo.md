# Todo List for (and by) Raiz Dev
## Build
- [x] Migrate to "Unity Build" style
- [ ] Create a 'build.sh' script (don't use Makefile)

## Errors
- [ ] Add functions for error and warning prints with a standardized message 
format
- [ ] Implement pretty (colorized) error printers(?)

## Debug
- [x] Add code for debug messages (either by macros or real functions)

## Preprocessor (macros)
- [ ] Implement a macro declaration, definition and expantion support for Raiz 
(what about it? Would be cool! Right?)
- [ ] Make it better than C's one (very challenging)!

## Lexer
- [x] Fix broken tokenizer (and organize the token kind and value handlers)!
- [x] Fix off-by-one errors about updating the current character index
- [x] Implement correct position spanning at tokens and lexer state
- [ ] Implement some kind of context remembering so that you can add NEW_LINE 
tokens only when needed

## Parser
- [ ] Be sure of what you want the Raiz syntax to be! 
(and what "programming paradigm(s)" it'll follow)
- [ ] Start implementing a parser
- [x] Define structs and enums for parser things (and some helpers also)
- [ ] Implement simple (math) operations and variable declaration and 
assignment parsing (don't worry about chained assignments yet, if it's not that 
easy to implement...)
- [ ] Think about that Hybrid Parser thing 
(iterating the tokens left to right and right to left depending on the context)
- [ ] Parse static typing at variables


## Semantics
- [ ] Start implementing a semantical checker (or maybe THE BORROW CHECKER)
- [ ] Think more about the language's features. (Because here is where they'll 
shine, at least most of them)

## Compiler
- [ ] Make a bytecode "compiler" (generator) for the Interpreter
- [ ] Be sure the bytecode looks like a custom ASSEMBLY
- [ ] Study about LLVM so we can use it to REALLY compile code! (compile into 
native assembly)
- [ ] Implement Raiz to LLVM IR transpiler(?)

## Interpreter
- [ ] Implement a simple interpreter. At least with math operations and 
variable storing (implement a hash table for such task)
