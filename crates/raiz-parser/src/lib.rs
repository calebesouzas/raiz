use raiz_core::{Expr, Operator, Program, RaizObject, Stmt, Token, TokenKind};

pub trait ProgramParser {
    fn build_ast(self) -> Program;
}

impl ProgramParser for Program {
    fn build_ast(mut self: Program) -> Program {
        let mut lexer = Lexer::new(&self.source);
        lexer.tokenize();

        let mut parser = Parser::new(lexer);
        parser.parse();

        self
    }
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

fn get_binding_power(op: &Operator) -> (u8, u8) {
    match *op {
        Operator::Sum | Operator::Subtract => (10, 11),
        Operator::Multiply | Operator::Divide => (20, 21),
        Operator::Unary => (30, 31),
        // _ => panic!("Unhandled operator: {:?}", op),
    }
}

#[derive(Debug, PartialEq)]
pub struct Parser {
    tokens: Vec<Token>,
}
impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        let mut tokens = lexer.tokens;
        tokens.reverse();

        Self { tokens }
    }
    pub fn parse(&mut self) -> Expr {
        println!("Tokens: {:?}", self.tokens);
        self.parse_expression(0)
    }
    fn peek(&self) -> &Token {
        &self.tokens.last().unwrap_or(&Token::EndOfFile)
    }
    fn next(&self) -> &Token {
        if self.tokens.len() >= 2 {
            &self.tokens[self.tokens.len() - 2]
        } else if self.tokens.len() != 0 {
            &self.tokens[1]
        } else {
            &self.tokens[0]
        }
    }
    fn advance(&mut self) {
        self.tokens.pop();
    }
    fn parse_expression(&mut self, minimum_bp: u8) -> Expr {
        println!("Called now");
        let current = self.peek();
        println!("Current token: {:?}", current);
        let mut left_side = match current {
            Token::NumberLiteral(number) => Expr::Literal(RaizObject::Int(*number)),
            Token::Minus => {
                let (_, bp) = get_binding_power(&Operator::Unary);
                self.advance();
                let right_side = self.parse_expression(bp);
                Expr::Unary {
                    operator: Operator::Subtract,
                    right: Box::new(right_side),
                }
            }
            Token::OpenParen => {
                self.advance();
                let inner = self.parse_expression(0);
                if self.peek() == &Token::CloseParen {
                    Expr::Group(Box::new(inner))
                } else {
                    panic!("Expected closing parenthesis");
                }
            }
            other => {
                panic!("Unhandled token: {:?}", other);
                Expr::Return(None)
            }
        };
        self.advance();
        loop {
            println!("At loop...");
            let current = self.peek();
            println!("Current token: {:?}", current);
            if current == &Token::EndOfFile {
                break;
            }
            let op = match current {
                Token::Plus => Operator::Sum,
                Token::Minus => Operator::Subtract,
                Token::Star => Operator::Multiply,
                Token::Slash => Operator::Divide,
                _ => break,
            };

            let (left_bp, right_bp) = get_binding_power(&op);

            if left_bp < minimum_bp {
                break;
            }

            self.advance();

            let right_side = self.parse_expression(right_bp);

            left_side = Expr::Binary {
                left: Box::new(left_side),
                operator: op,
                right: Box::new(right_side),
            };
        }
        left_side
    }
}
