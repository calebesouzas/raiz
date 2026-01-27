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
    grouping_test();
}
#[allow(unused)]
fn one_plus_one_test() {
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
    dbg!(&actual_result);
    assert_eq!(actual_result, wanted_result);
}
fn grouping_test() {
    println!("At grouping test!");
    let code = "15 * ((2 + 7) - 8 / 2)";
    println!("Code: {}", code);
    let mut lexer = Lexer::new(&code.to_string());
    lexer.tokenize();

    println!("Tokens: {:?}", lexer.tokens);

    let mut parser = Parser::new(lexer);
    let result = parser.parse();

    dbg!(result);
}
