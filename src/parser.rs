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
    pub fn new(source: &String) -> Result<Self, String> {
        let chars = source.chars().collect::<Vec<char>>();
        let mut lexer = Lexer::new(chars);

        let mut next_token = lexer.next_token()?;
        let current_token = next_token.clone();
        next_token = lexer.next_token()?;

        Ok(Self {
            lexer,
            current_token,
            next_token,
            ast: Expr::Void,
        })
    }

    pub fn parse(mut self) -> Result<Expr, String> {
        self.ast = self.parse_expr(0)?;
        Ok(self.ast)
    }

    fn parse_nud(&mut self) -> Result<Expr, String> {
        match self.current() {
            Token::Error{message} => Err(message.to_owned()),
            Token::Number{value} => {
                let result = Expr::Literal{value: *value};
                self.advance();
                Ok(result)
            }
            Token::Minus => {
                self.advance();

                let (_, right_power) = get_bind_power(&Token::Minus);

                let expr = self.parse_expr(right_power)?;
                Ok(Expr::Unary { target: Box::new(expr) })
            }
            Token::OpenParen => {
                self.advance();

                let inner = self.parse_expr(0)?;

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
        let mut left_side = self.parse_nud()?;

        while !self.expect(Token::EndOfFile) && !self.expect(Token::CloseParen) {
            let op = {
                if !self.operator_token(&self.current()) {
                    let message: String = 
                        format!("expected operator, found {:?}", self.current());
                    return Err(message);
                }

                self.current().clone()
            };

            let (left_power, right_power) = get_bind_power(&op);
            if left_power < minimum_power {
                break;
            }

            self.advance(); // consume operator here

            let right_side = self.parse_expr(right_power)?;

            left_side = Expr::Binary {
                left: Box::new(left_side),
                right: Box::new(right_side),
                op,
            } // ';' is optional here, since we'll return `left_side` anyways
        }

        Ok(left_side)
    }

    //////// Helpers ////////
    // Looks like the borrow checker doesn't like helper functions.
    // With so many mutable and also immutable borrows at the same time, it is getting crazy...
    fn current(&self) -> &Token {
        &self.current_token
    }

    fn peek(&self) -> &Token {
        &self.next_token
    }

    fn next(&mut self) -> &Token {
        self.advance();
        &self.current()
    }

    fn advance(&mut self) {
        self.current_token = self.next_token.clone();
        self.next_token = {
            let result = self.lexer.next_token();
            match result {
                Ok(token) => token,
                Err(message) => Token::Error{message},
            }
        };
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
