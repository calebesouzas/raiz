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
  val name @string!
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
