use crate::lexer::{Token, Lexer};
use crate::ast::*;

fn get_bind_power(token: &Token) -> (usize, usize) {
    match *token {
        Token::Plus | Token::Minus => (1, 2),
        Token::Star | Token::Slash => (3, 4),
        _ => (0, 0),
    }
}

pub struct Parser {
    pub lexer: Lexer, // yeah, it owns the lexer

    current_token: Token,
    next_token: Token,
    ast: Expr,
}

impl Parser {
    pub fn new(source: &String) -> Self {
        let chars = source.chars().collect::<Vec<char>>();
        let mut lexer = Lexer::new(chars);

        let mut next_token = lexer.next_token().expect("failed to generate token");
        let current_token = next_token;
        next_token = lexer.next_token().expect("failed to generate token");

        Self {
            lexer,
            current_token,
            next_token,
            ast: Expr::Void,
        }
    }

    pub fn parse(&mut self) -> Expr {
        self.parse_expr(0).expect("failed to parse expression")
    }

    fn parse_nud(&mut self) -> Result<Expr, String> {
        match self.current() {
            Token::Number{value} => {
                self.advance();
                Ok(Expr::Literal{value})
            }
            Token::Minus => {
                self.advance();

                let (_, right_power) = get_bind_power(&Token::Minus);

                let expr = self.parse_expr(right_power).expect("failed to parse unary expression");
                Ok(Expr::Unary { target: Box::new(expr) })
            }
            Token::OpenParen => {
                self.advance();

                let inner = self.parse_expr(0).expect("failed to parse inner expression");

                if !self.expect(Token::CloseParen) {
                    return Err(format!("expected ')' before {:?}", self.peek()));
                }

                self.advance();

                Ok(inner)
            }
            Token::CloseParen => Err(format!("trailing ')' before {:?}", self.peek())),

            // Shoudn't 'panic()'... But i really don't know how to deal with it when i have to
            // return something... Maybe some kind of 'Expr::Void' or 'Expr::Unit' would help?
            Token::EndOfFile => Err(format!("unexpected end of file")),

            Token::Plus | Token::Star | Token::Slash => unreachable!("operator at 'parse_nud"),
        }
    }

    fn parse_expr(&mut self, minimum_power: usize) -> Result<Expr, String> {
        let mut left_side = self.parse_nud().expect("failed to parse null-denotation expression");

        while !self.expect(Token::EndOfFile) && !self.expect(Token::CloseParen) {
            let op = {
                if !self.operator_token(&self.current()) {
                    let message: String = format!("expected operator, found {:?}", self.current());
                    return Err(message);
                }

                self.current()
            };

            self.advance();

            let (left_power, right_power) = get_bind_power(&op);

            if left_power < minimum_power {
                break;
            }

            let right_side = self.parse_expr(right_power)
                .expect("failed to parse binary right side");

            left_side = Expr::Binary {
                left: Box::new(left_side),
                right: Box::new(right_side),
                op,
            }
        }

        Ok(left_side)
    }

    //////// Helpers ////////
    // Looks like the borrow checker doesn't like helper functions.
    // With so many mutable and also immutable borrows at the same time, it is getting crazy...
    fn current(&self) -> Token {
        self.current_token
    }

    fn peek(&self) -> Token {
        self.next_token
    }

    fn next(&mut self) -> Token {
        self.advance();
        self.current()
    }

    fn advance(&mut self) {
        self.current_token = self.next_token;
        self.next_token = self.lexer.next_token().expect("failed to generate token");
    }

    fn expect(&self, expected: Token) -> bool {
        self.current_token == expected
    }

    fn expect_next(&self, expected: Token) -> bool {
        self.next_token == expected
    }

    fn operator_token(&self, token: &Token) -> bool {
        match *token {
            Token::Plus | Token::Minus | Token::Star | Token::Slash => true,
            _ => false,
        }
    }
}
