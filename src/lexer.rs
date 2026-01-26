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
    current: usize,
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
            current: 0,
            tokens: Vec::<Token>::new(),
        }
    }
    fn peek(&self) -> Option<&Token> {
        self.tokens.last()
    }
    fn next(&mut self) -> Option<Token> {
        let c = self.chars[self.current];
        let result = match c {
            '+' => Some(Token::Plus),
            '-' => Some(Token::Minus),
            '*' => Some(Token::Star),
            '/' => Some(Token::Slash),
            '(' => Some(Token::OpenParen),
            ')' => Some(Token::CloseParen),
            '0'..='9' => {
                let number = self.get_number();
                Some(Token::NumberLiteral(number))
            }
            ' ' | '\n' | '\t' | '\r' => None,
            other => {
                panic!("Invalid character: {}", other);
            }
        };
        result
    }
    fn advance(&mut self, amount: usize) {
        if self.current + amount < self.chars.len() {
            self.current += amount;
        }
    }
    pub fn tokenize(&mut self) {
        while self.current < self.chars.len() {
            if let Some(token) = self.next() {
                self.tokens.push(token);
                self.advance(1);
            } else if self.chars[self.current].is_ascii_whitespace() {
                self.advance(1);
            }
        }
        self.tokens.push(Token::EndOfFile);
    }
    fn get_number(&mut self) -> i32 {
        let mut number: i32 = 0;
        // Pass a reference for 'chars' starting at the current one
        let cs = &self.chars[self.current..];

        let mut iter = cs.iter().take_while(|c| c.is_ascii_digit());

        while let Some(c) = iter.next() {
            number = number * 10 + (*c as i32 - 48);
        }
        self.advance(iter.count());
        // while self.current < self.chars.len() {
        //     let current = self.chars[self.current];
        //     if current.is_ascii_digit() {
        //         // 48 is the decimal code for '0'
        //         number = number * 10 + (current as i32 - 48);
        //         self.advance(1);
        //     } else {
        //         break;
        //     }
        // }
        // self.current -= 1;
        number
    }
}
