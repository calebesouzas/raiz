#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Plus,
    Minus,
    Star,
    Slash,
    OpenParen,
    CloseParen,
    Number { value: i32 },
    EndOfFile,

    Error { message: String },
}

pub struct Lexer {
    chars: Vec<char>,
    pub line: usize,
    pub column: usize,
    start: usize,
    current: usize,
}

impl Lexer {
    pub fn new(chars: Vec<char>) -> Self {
        Self {
            chars,
            line: 1,
            column: 1,
            start: 0,
            current: 0
        }
    }
    fn skip_whitespace(&mut self) {
        while let Some(c) = self.chars.get(self.current) && c.is_whitespace() {
            self.current += 1;
        }
    }

    pub fn next_token(&mut self) -> Result<Token, String> {
        self.skip_whitespace();

        self.start = self.current;

        let c = self.advance();

        let token = match c {
            '+' => Token::Plus,
            '-' => Token::Minus,
            '*' => Token::Star,
            '/' => Token::Slash,
            '(' => Token::OpenParen,
            ')' => Token::CloseParen,
            '\0' => Token::EndOfFile,
            '0'..='9' => self.number(),
            other => return Err(format!("unexpected character: {}", other)),
        };

        dbg!(&token);
        Ok(token)
    }

    fn number(&mut self) -> Token {
        let mut number: i32 = self.prev() as i32 - 48; // don't skip the first digit!

        while self.peek().is_ascii_digit() {
            number = (number * 10) + ((self.advance() as i32) - 48);
        }

        Token::Number { value: number }
    }

    fn advance(&mut self) -> char {
        let c = self.chars.get(self.current).unwrap_or(&'\0');
        self.current += 1;

        if let '\n' = *c {
            self.line += 1;
            self.column = 1;
        } else {
            self.column += 1;
        }

        *c
    }

    fn peek(&self) -> char {
        *self.chars.get(self.current).unwrap_or(&'\0')
    }

    fn prev(&self) -> char {
        *self.chars.get(self.current - 1).unwrap_or(&'\0')
    }

    fn next(&self) -> char {
        *self.chars.get(self.current + 1).unwrap_or(&'\0')
    }

}
