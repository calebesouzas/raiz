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
        let mut left_side = match self.peek() {
            Token::NumberLiteral(number) => Expr::Literal(Value::Int(*number)),
            Token::Minus => {
                let (_, bp) = get_binding_power(&Operator::Unary);
                self.advance();
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
