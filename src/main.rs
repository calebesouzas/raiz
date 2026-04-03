mod lexer;
mod parser;
mod ast;
use parser::Parser;

fn main() {
    let source = String::from("1 + 2 * 3\n");

    let mut parser = Parser::new(&source);
    let ast = parser.parse();

    println!("Result: {}", ast.eval());
}
