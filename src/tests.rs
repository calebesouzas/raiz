use crate::lexer::Lexer;

#[test]
fn test_lexer_tokens_empty() {
    let mut lexer = Lexer::new("var x = 10!");
    lexer.tokenize();
    assert!(!lexer.tokens.is_empty());
}
