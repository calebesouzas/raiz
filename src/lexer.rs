pub enum Token<'a> {
    Identifier(&'a str),
    BinaryOperator(char),
    NumberLiteral(i32),
    FloatingNumber(f32),
}

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,
    tokens: Vec<Token<'a>>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        Self {
            input,
            position: 0,
            tokens: vec![],
        }
    }

    pub fn tokenize(&mut self) {
        loop {
            let ch: &Option<char> = &self.input.chars().nth(self.position);
            match ch {
                Some(c) => match c {
                    'a'..='z' | 'A'..='Z' => self.get_identifier(),
                    other => panic!("Unhandled character: {}", other),
                },
                None => break,
            }
        }
    }
    fn get_identifier(&mut self) {
        let start = self.position;
        while let Some(ch) = self.input.chars().nth(self.position) {
            if ch.is_alphanumeric() || ch == '_' {
                self.position += 1;
            } else {
                break;
            }
        }
        let identifier = &self.input[start..self.position];
        self.tokens.append(&mut Token::Identifier(identifier));
    }
}
