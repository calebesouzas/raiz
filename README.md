# Raiz
## Description
A programming language focused in easier string manipulation, more complex math 
operations and great shell integration.

## Meaning
Just a brief explaination of what "raiz" (pronounced like "ha is") means:
In Brazil, it is a slang for people who do things with no frills, sometimes in 
a _hardcore_ way. At least this is the meaning i wanted to use...

## Important
This is just a prototype! So please, don't expect the code to work, be well 
optimized and secure. I'm currently racing to get a simple and working Raiz to 
C transpiler and that's it.

## How to try
### Cloning
Just run the following commands:
```console
git clone https://github.com/calebesouzas/raiz.git
cd raiz
```

### Compiling
Compiling Raiz should be simple. You can use the compiler of your choice. 
You'll just need to set output executable name and insert `./source` folder to 
`#include` path somehow.

### Clang Example
A valid command for Unix-like systems (as far as i know): 
```console
cc -x c -I./source -o raiz raiz.c
```
You can also pass warning flags like `-Wall -Wextra` if you want.

### Hello World Program
Create a `hello.rz` file and try this code:
```text
use stdio

fun main = @int
{
  printf! "Hello, World!\n"
  return 0
}
```
### Running
It's rather simple, just write C code using Raiz syntax. Then, you run the 
following command:
```console
./raiz hello.rz
```
It should create a `hello` executable.

### Auto Run and CLI Flags
You can also provide a `+run` flag to `raiz` command to auto-run and optionaly 
pass flags. Just like this:
```console
./raiz <file_name.rz> +run <flags for your executable...>
```

## Roadmap
Check `PROGRESS.md` for roadmap and implementation progress details.

## Contributing
I'm not accepting direct contributions (pull requests), but comments, tips and 
any other kind of help are welcome and i really appreciate them!

## Transpiler Coding Style
Even knowing i won't accept pull requests, maybe you could find a cool 
C programming style (but probably not...).

Better details should be at `STYLE.md`.

## Raiz Coding Style
I want it to be different (but not unique, since it's actually very hard to do 
these days). Including many things i like from other languages.

I'm not a desingner or anything like that, but i trust the programming essence!

Better details should be at `.rz` files inside `examples/` folder.

## License
There is literally no license, you can do whatever you want with the code! But 
please don't steal the language name... I really don't want to lose this one, 
i found a great oportunity with it!

## Thanks
Thank you for caming up and looking at this project! I know it has potencial... 
but maybe not in my hands, at least not now. I'm not a very good programmer 
and i actually procrastinate too much sometimes (by the way, all of this is a 
rather complex thing), so the development will probably be very slow.

