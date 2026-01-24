use crate::lexer::{Lexer, Token};

#[test]
fn negative_number_literals() {
    let code = String::from("-15");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    assert_eq!(lexer.tokens[0], Token::NumberLiteral(-15));
}
