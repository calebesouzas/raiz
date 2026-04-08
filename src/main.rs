use std::io::{self, Write};

mod lexer;
mod parser;
mod ast;
use parser::Parser;

fn main() {
    loop {
        print!("&> ");

        io::stdout().flush().expect("failed to flush stdout");

        let mut source = String::new();
        io::stdin()
            .read_line(&mut source)
            .expect("failed to read line");

        let ast = Parser::new(&source).parse();

        println!("Result: {}", ast.eval());
    }
}
