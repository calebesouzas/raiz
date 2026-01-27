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
    #[allow(unused_assignments)]
    pub fn tokenize(&mut self) {
        let mut i = 0;
        while i < self.chars.len() {
            let c = self.chars[i];
            let mut token = Token::EndOfFile;
            match c {
                '+' => token = Token::Plus,
                '-' => token = Token::Minus,
                '*' => token = Token::Star,
                '/' => token = Token::Slash,
                '(' => token = Token::OpenParen,
                ')' => token = Token::CloseParen,
                '0'..='9' => {
                    let mut number = c as i32 - 48;
                    //let mut number: i32 = 0;
                    while self.chars[i + 1].is_ascii_digit() {
                        let ch = self.chars[i + 1];
                        number = number * 10 + (ch as i32 - 48);
                        i += 1;
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
            i += 1;
        }
        self.tokens.push(Token::EndOfFile);
    }
}
