use crate::lexer::{Lexer, Token};

#[derive(Debug, PartialEq)]
pub enum Expr {
    Literal(Value),
    Variable(String),
    Binary {
        left: Box<Expr>,
        operator: Operator,
        right: Box<Expr>,
    },
    Unary {
        operator: Operator,
        right: Box<Expr>,
    },
    Group(Box<Expr>),
    Return(Option<Value>),
}

#[derive(Debug, PartialEq)]
pub enum Operator {
    Sum,
    Subtract,
    Multiply,
    Divide,
    Unary,
    // And others like Access, Index(?) and more...
}

#[derive(Debug, PartialEq)]
pub enum Value {
    Int(i32),
    Float(f32),
    Char(char),
    Size(usize),
    String(String),
}

fn get_binding_power(op: &Operator) -> (u8, u8) {
    match *op {
        Operator::Sum | Operator::Subtract => (10, 10),
        Operator::Multiply | Operator::Divide => (20, 20),
        Operator::Unary => (30, 30),
        // _ => panic!("Unhandled operator: {:?}", op),
    }
}

#[derive(Debug, PartialEq)]
pub struct Parser {
    tokens: Vec<Token>,
    current: usize,
}
impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        Self {
            tokens: lexer.tokens,
            current: 0,
        }
    }
    pub fn parse(&mut self) -> Expr {
        self.parse_expression(0)
    }
    fn peek(&self) -> &Token {
        let index = {
            if self.current < self.tokens.len() {
                self.current
            } else {
                0
            }
        };
        &self.tokens[index]
    }
    fn next(&mut self) -> &Token {
        let index = {
            if self.current + 1 < self.tokens.len() {
                self.current + 1
            } else {
                self.current
            }
        };
        &self.tokens[index]
    }
    fn advance(&mut self) {
        self.current += 1;
    }
    fn parse_expression(&mut self, minimum_bp: u8) -> Expr {
        let mut left_side = match self.peek() {
            Token::NumberLiteral(number) => Expr::Literal(Value::Int(*number)),
            Token::Minus => {
                let (_, bp) = get_binding_power(&Operator::Unary);
                let right_side = self.parse_expression(bp);
                Expr::Unary {
                    operator: Operator::Subtract,
                    right: Box::new(right_side),
                }
            }
            other => {
                println!("Unhandled token: {:?}", other);
                Expr::Return(None)
            }
        };
        self.advance();
        loop {
            if self.peek() == &Token::EndOfFile {
                break;
            }
            let op = match self.peek() {
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
