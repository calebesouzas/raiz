# The Raiz Syntax
I am really thoughtful about this language's syntax. 
I will try to not make a bad syntax. I'm not sure about what is a bad syntax, 
could be something like Go does to separate private and public functions, 
types and struct fields, etc; or even the use of `::` as access 
operator in Rust or C++; the use of `;` to end statements in 
many languages; the use of `<` and `>` at generic types. Well... About the 
statement end marker, i can't remove it totally because this language will be 
based on expressions, then, i need to have an easy way to separate statements 
of expressions. But i can replace it with another symbol (will be `!`).

### Some examples of the syntax
I have many ideas for designing the syntax... Here are some examples:
```
tab Person {
  val name @string! // '!' marks end of statements
  var age @int!
  met birthday {
    self.age += 1!
    print($"{self.name} is now {self.age} years old!")!
  }
  fun born(name @string) self {
    { age = 0, name }
  }
  onevent new => born!
}

fun main(args [@string]) {
  var me @Person = "Calebe"!
  me.birthday()!
}
```
Or i could use this:
```
tab Person {
  val name @string
  var age @int
}

fun @Person.born name @string => @Person { ; can also use '@self'
  ; '!' marks function calls (parenthesis are optional)
  ; and by default, the value of the last expression is returned.
  ; But you can also return earlier with 'return' keyword

  @Person! { name, .age = 0 } ; this calls the type constructor

  ; the constructor just assigns the values to their respective field.
  ; We will have a syntax sugar like the one we used with field 'name'
}

met @Person.birthday => {
  self.age++
}
; I could also allow function and "method" declarations inside their respective
; tables. But with this syntax, i'll wanted to use Entity Component System
; the famous ECS programming style instead of OOP. Because it must be a little
; easier to work with and it would probably be more optimized...

fun main args @[string] => {
  var me = @Person.born! "Calebe"
  me.birthday!
}
```
You noticed that in this one i used ';' for comments, but don't worry. I'll 
allow use of '//' too. And there are many differences... At first i really 
thought that the first one would be amazing, but i realised: it's still 
forcing some kind of character to mark statement endings. And i don't want it 
to happen! I hate when my code doesn't compile just due to a missing ';'... 
So in this syntax, new lines will mark end of statements (but of course, the 
lexer or parser need to smart enough and not put 'TOKEN_NEW_LINE' everywhere).
