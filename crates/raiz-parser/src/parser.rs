use crate::lexer;
use raiz_core::Error;
use raiz_core::{Cursor, Expr, Object, Stmt, Token, Token::*};

pub struct Parser {
    tokens: Cursor<Token>,
    ast: Vec<Stmt>,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Self {
            tokens: Cursor::new(tokens),
            ast: Vec::<Stmt>::new(),
        }
    }
    pub fn parse(&mut self) -> Result<Vec<Stmt>, Error> {
        let mut ast = Vec::<Stmt>::new();
        while let Some(token) = self.tokens.current() {
            let stmt = match token {
                KeywordVar => self.parse_var_declaration(),
                other => {
                    return Err(Error {
                        msg: format!("Unexpected token: {}", other),
                        pos: None,
                    });
                }
            };
            match stmt {
                Ok(statement) => ast.push(statement),
                Err(e) => return Err(e),
            }
        }
        Ok(ast)
    }
    fn parse_var_declaration(&mut self) -> Result<Stmt, Error> {
        self.tokens.advance();
        let result = {
            if let Some(Identifier(iden)) = self.tokens.current() {
                self.tokens.advance();
                match self.tokens.current().clone().unwrap() {
                    Collon => {
                        if let Some(Identifier(defined_type)) = self.tokens.peek() {
                            Stmt::VarDeclaration {
                                name: *iden,
                                init: None,
                                defined_type: Some(*defined_type),
                            }
                        } else {
                            return Err(Error {
                                msg: "Expected type name after identifier".to_string(),
                                pos: None,
                            });
                        }
                    }
                    Assign => {
                        let init_expr = self.parse_expression(0);
                    }
                }
            } else {
                Err(Error {
                    msg: "Expected identifier after 'var'".to_string(),
                    pos: None,
                })
            }
        };
        result
    }
    fn parse_expression(&mut self, min_bp: usize) -> Expr {
        // NOTE: aparently, 'self.tokens.current'
        // is not being updated correctly
        let left_or_main = {
            if let Some(token) = self.tokens.peek() {
                match token {
                    NumberLiteral(num) => Expr::Literal(Object::Int(*num)),
                    CharLiteral(ch) => Expr::Literal(Object::Char(*ch)),
                    StringLiteral(s) => Expr::Literal(Object::String(s.to_owned())),
                    BooleanLiteral(b) => Expr::Literal(Object::Bool(*b)),
                    Identifier(iden) => Expr::Variable(*iden),
                    OpenParen => {
                        let expr = self.parse_expression(0);
                        if let Some(CloseParen) = self.tokens.current() {
                            expr
                        } else {
                            Expr::Literal(Object::None)
                            // TODO: handle syntax-error: expexted ')'
                        }
                    }
                }
            } else {
                Expr::Literal(Object::None)
            }
        };

        left_or_main
    }
}
