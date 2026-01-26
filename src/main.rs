use std::env;

#[cfg(test)]
mod tests;

mod lexer;
use lexer::Lexer;

mod parser;
use parser::{Expr, Operator, Parser, Value};

fn main() {
    let version = "v0.0.1-test";
    let args = env::args();

    if args.len() == 1 {
        println!("Raiz language - {}", version);
    }

    let code = String::from("-7");

    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    dbg!(&lexer.tokens);

    let mut parser = Parser::new(lexer);
    let ast = parser.parse();
    dbg!(ast);

    parser_test_im_scaried();
}
fn parser_test_im_scaried() {
    let code = String::from("1 + 1");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    let mut parser = Parser::new(lexer);
    let actual_result = parser.parse();

    let wanted_result = Expr::Binary {
        left: Box::new(Expr::Literal(Value::Int(1))),
        operator: Operator::Sum,
        right: Box::new(Expr::Literal(Value::Int(1))),
    };
    assert_eq!(actual_result, wanted_result);
}
