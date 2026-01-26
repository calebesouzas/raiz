use crate::lexer::{Lexer, Token};
use crate::parser::{Expr, Operator, Parser, Value};

#[test]
fn not_empty() {
    let code = String::from("10 + 5 - 7");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    assert!(!lexer.tokens.is_empty());
}

#[test]
fn negative_number_literals() {
    let code = String::from("-15");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    assert_eq!(lexer.tokens[0], Token::Minus);
    assert_eq!(lexer.tokens[1], Token::NumberLiteral(15));
}

#[test]
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

#[test]
fn parse_unary_operations() {
    let code = String::from("-15");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    let mut parser = Parser::new(lexer);
    let result = parser.parse();

    let wanted_result = Expr::Unary {
        operator: Operator::Subtract,
        right: Box::new(Expr::Literal(Value::Int(15))),
    };

    dbg!(result);
    //assert_eq!(result, wanted_result);
}
