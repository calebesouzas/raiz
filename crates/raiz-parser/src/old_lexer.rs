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
            let mut token = TokenKind::EndOfFile;
            match c {
                '+' => token = TokenKind::Plus,
                '-' => token = TokenKind::Minus,
                '*' => token = TokenKind::Star,
                '/' => token = TokenKind::Slash,
                '(' => token = TokenKind::OpenParen,
                ')' => token = TokenKind::CloseParen,
                '0'..='9' => {
                    let mut number = c as i32 - 48;
                    //let mut number: i32 = 0;
                    while self.chars[i + 1].is_ascii_digit() {
                        let ch = self.chars[i + 1];
                        number = number * 10 + (ch as i32 - 48);
                        i += 1;
                    }

                    token = TokenKind::NumberLiteral(number);
                }
                other => {
                    if !other.is_ascii_whitespace() {
                        panic!("Invalid character: {}", other);
                    }
                }
            }
            if token != TokenKind::EndOfFile {
                self.tokens.push(token);
            }
            i += 1;
        }
        self.tokens.push(TokenKind::EndOfFile);
    }
}
