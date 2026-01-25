use crate::lexer::{Lexer, Token};

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
