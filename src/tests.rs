use crate::lexer::Lexer;

#[test]
fn test_lexer_tokens_empty() {
    let test = String::from("var x = 10!");
    let mut lexer = Lexer::new(&test);
    lexer.tokenize();
    assert!(!lexer.tokens.is_empty());
}
