use crate::lexer::{Lexer, Token};

#[test]
fn test_statement_end_token() {
    let test = String::from("var x = 10!");
    let mut lexer = Lexer::new(&test);
    lexer.tokenize();

    let before_last = {
        let len = lexer.tokens.len();
        if len >= 2 {
            lexer.tokens.get(len - 2)
        } else {
            None
        }
    };
    let last = lexer.tokens.last();
    assert_eq!(last, Some(&Token::EndOfFile));
    assert_eq!(before_last, Some(&Token::StatementEnd));
}

#[test]
fn negative_number_literals() {
    let code = String::from("-15");
    let mut lexer = Lexer::new(&code);
    lexer.tokenize();

    assert_eq!(lexer.tokens[0], Token::NumberLiteral(-15));
}
