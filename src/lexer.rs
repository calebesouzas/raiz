#[derive(Debug, PartialEq, Eq, Clone)]
pub enum Token<'a> {
    Identifier(&'a str),
    Keyword(&'a str),
    BinaryOperator(char),
    NumberLiteral(i32),
    StringLiteral(&'a str),
    NotEqual,
    Equal,
    StatementEnd,
    Assign,
    EndOfFile,
}

#[derive(Debug)]
pub struct Lexer<'a> {
    input: &'a String,
    bytes: &'a [u8],
    position: usize,
    pub tokens: Vec<Token<'a>>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a String) -> Self {
        Self {
            input,
            bytes: input.as_bytes(),
            position: 0,
            tokens: Vec::<Token>::new(),
        }
    }
    fn peek_prev(&self) -> Option<char> {
        if self.position - 1 < self.bytes.len() {
            Some(self.bytes[self.position - 1] as char)
        } else {
            None
        }
    }
    fn peek_next(&self) -> Option<char> {
        if self.position + 1 < self.bytes.len() {
            Some(self.bytes[self.position + 1] as char)
        } else {
            None
        }
    }
    fn peek(&self) -> Option<char> {
        if self.position < self.bytes.len() {
            Some(self.bytes[self.position] as char)
        } else {
            None
        }
    }
    fn advance(&mut self) {
        self.position += 1;
    }
    pub fn tokenize(&mut self) {
        loop {
            if let Some(c) = self.peek() {
                match c {
                    'a'..='z' | 'A'..='Z' => self.get_identifier(),
                    '0'..='9' => self.get_number(),
                    '\"' => self.get_string(),
                    '+' | '-' | '*' | '/' => self.tokens.push(Token::BinaryOperator(c)),
                    '!' => match self.peek_next() {
                        Some('=') => {
                            self.tokens.push(Token::NotEqual);
                            self.advance();
                        }
                        _ => self.tokens.push(Token::StatementEnd),
                    },

                    '=' => match self.peek_next() {
                        Some('=') => {
                            self.tokens.push(Token::Equal);
                            self.advance();
                        }
                        _ => self.tokens.push(Token::Assign),
                    },
                    ' ' | '\n' | '\r' | '\t' => {
                        self.advance();
                        continue;
                    }
                    other => panic!("Unhandled character: {}", other),
                }
            } else {
                self.tokens.push(Token::EndOfFile);
                break;
            }
            self.advance();
        }
    }
    fn get_identifier(&mut self) {
        let start = self.position;
        while let Some(c) = self.peek() {
            if c.is_alphanumeric() || c == '_' {
                self.advance();
            } else {
                break;
            }
        }
        let word = &self.input[start..self.position];
        let token = match word {
            "var" | "let" => Token::Keyword(word),
            _ => Token::Identifier(word),
        };
        self.tokens.push(token);
        self.position -= 1;
    }
    fn get_number(&mut self) {
        let mut number: i32 = 0;
        let modifier = { if self.peek_prev() == Some('-') { -1 } else { 1 } };
        while let Some(c) = self.peek() {
            if c.is_numeric() {
                number = number * 10 + (c as i32 - 48);
                self.position += 1;
            } else {
                break;
            }
        }
        number *= modifier;
        self.tokens.push(Token::NumberLiteral(number));
        self.position -= 1;
    }
    fn get_string(&mut self) {
        let start = self.position;
        while let Some(c) = self.peek() {
            if c != '\"' {
                self.advance();
            } else {
                break;
            }
        }
        let string_literal = &self.input[start..self.position];
        self.tokens.push(Token::StringLiteral(string_literal));
    }
}
