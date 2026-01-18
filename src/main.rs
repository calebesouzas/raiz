use std::env;
// use std::process::exit; // for 'exit()' function

pub enum Token {
    NumberLiteral(i32),
    BinaryOperator(char),
    Identifier(String),
    SpecialIdentifier(String),
    OpenGroup(char),
    CloseGroup(char),
    Assignment,
    LogicalOperator(String),
    StatementEnd,
    Eof,
}

pub struct Lexer {
    input: Vec<char>,
    position: usize,
}

impl Lexer {
    pub fn new(input: &str) -> Self {
        Self {
            input: input.chars().collect(),
            position: 0,
        }
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        if self.position >= self.input.len() {
            return Token::Eof;
        }

        let ch = self.input[self.position];

        let tok = match ch {
            '#' => Token::SpecialIdentifier,
        };
    }
}

fn main() {
    let version = "v0.0.1-test";
    let args: Vec<String> = env::args().collect();

    if args.len() == 1 {
        println!("Raiz language - {}", version);
        println!("Use 'quit()' function to exit...");
    }
}
