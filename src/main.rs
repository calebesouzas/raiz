use std::env;

#[cfg(test)]
mod tests;

mod lexer;
use lexer::Lexer;

mod parser;
use parser::Parser;

fn main() {
    let version = "v0.0.1-test";
    let args = env::args();

    if args.len() == 1 {
        println!("Raiz language - {}", version);
    }

    let code = String::from("15 + 3 - 7");

    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    dbg!(&lexer.tokens);

    let mut parser = Parser::new(lexer);
    let ast = parser.parse();
    dbg!(ast);
}
