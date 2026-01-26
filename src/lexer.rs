#[derive(Debug, PartialEq, Eq, Clone)]
pub enum Token {
    Plus,
    Minus,
    Star,
    Slash,
    OpenParen,
    CloseParen,
    NumberLiteral(i32),
    EndOfFile,
}

#[derive(Debug)]
pub struct Lexer {
    chars: Vec<char>,
    pub tokens: Vec<Token>,
}

impl<'a> Lexer {
    pub fn new(input: &'a String) -> Self {
        let mut chars = Vec::<char>::new();
        let mut iter = input.chars().into_iter();
        while let Some(ch) = iter.next() {
            chars.push(ch);
        }
        Self {
            chars,
            tokens: Vec::<Token>::new(),
        }
    }
    pub fn tokenize(&mut self) {
        let mut chars = self.chars.iter();
        while let Some(c) = chars.next() {
            #[allow(unused_assignments)]
            let mut token = Token::EndOfFile;
            match c {
                '+' => token = Token::Plus,
                '-' => token = Token::Minus,
                '*' => token = Token::Star,
                '/' => token = Token::Slash,
                '(' => token = Token::OpenParen,
                ')' => token = Token::CloseParen,
                '0'..='9' => {
                    let mut number = *c as i32 - 48;
                    while let Some(ch) = chars.next()
                        && ch.is_ascii_digit()
                    {
                        number = number * 10 + (*ch as i32 - 48);
                    }
                    token = Token::NumberLiteral(number);
                }
                other => {
                    if !other.is_ascii_whitespace() {
                        panic!("Invalid character: {}", other);
                    }
                }
            }
            if token != Token::EndOfFile {
                self.tokens.push(token);
            }
        }
        self.tokens.push(Token::EndOfFile);
    }
}
