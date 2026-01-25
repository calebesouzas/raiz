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
    fn peek(&self) -> Option<&Token> {
        self.tokens.last()
    }
    fn next(&mut self) -> Option<Token> {
        let c = self.bytes[self.current] as char;
        let result = match c {
            '+' => Some(Token::Plus),
            '-' => Some(Token::Minus),
            '*' => Some(Token::Star),
            '/' => Some(Token::Slash),
            '(' => Some(Token::OpenParen),
            ')' => Some(Token::CloseParen),
            '0'..='9' => {
                let number = self.get_number();
                // -1 so we don't skip one character
                // right after the end of the number
                self.current -= 1;
                Some(Token::NumberLiteral(number))
            }
            other => {
                println!("Invalid character: {}", other);
                None
            }
        };
        self.current += 1;
        result
    }
    pub fn tokenize(&mut self) {
        while self.current < self.bytes.len() {
            if self.peek() != Some(&Token::EndOfFile) {
                let token = self.next().unwrap_or(Token::EndOfFile);

                self.tokens.push(token);
                self.current += 1;
            } else {
                break;
            }
        }
    }
    fn get_number(&mut self) -> i32 {
        let mut number: i32 = 0;
        while self.current < self.bytes.len() {
            let c = self.bytes[self.current] as char;
            if c.is_ascii_digit() {
                // 48 is the decimal code for '0'
                number = number * 10 + (c as i32 - 48);
                self.current += 1;
            } else {
                break;
            }
        }
        number
    }
}
