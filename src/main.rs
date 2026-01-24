use std::env;
mod lexer;

#[cfg(test)]
mod tests;

use lexer::Lexer;

fn main() {
    let version = "v0.0.1-test";
    let args = env::args();

    if args.len() == 1 {
        println!("Raiz language - {}", version);
    }

    let code = String::from("var x = 10!");

    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    dbg!(lexer.tokens);
}
