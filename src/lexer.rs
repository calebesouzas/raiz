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
pub struct Lexer<'a> {
    // input: &'a String,
    bytes: &'a [u8],
    current: usize,
    pub tokens: Vec<Token>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a String) -> Self {
        Self {
            // input,
            bytes: input.as_bytes(),
            current: 0,
            tokens: Vec::<Token>::new(),
        }
    }
    fn next(&mut self) -> Token {
        let c = self.bytes[self.current] as char;
        let result = match c {
            '+' => Token::Plus,
            '-' => Token::Minus,
            '*' => Token::Star,
            '/' => Token::Slash,
            '(' => Token::OpenParen,
            ')' => Token::CloseParen,
            '0'..='9' => {
                let number = self.get_number();
                self.current -= 1;
                Token::NumberLiteral(number)
            }
            _ => Token::EndOfFile,
        };
        self.current += 1;
        result
    }
    pub fn tokenize(&mut self) {
        while self.current < self.bytes.len() {
            let token = self.next();
            self.tokens.push(token);
            self.current += 1;
        }
    }
    fn get_number(&mut self) -> i32 {
        let mut number: i32 = 0;
        while self.current < self.bytes.len() {
            let c = self.bytes[self.current] as char;
            if c.is_numeric() {
                number = number * 10 + (c as i32 - 48);
                self.current += 1;
            } else {
                break;
            }
        }
        number
    }
}
