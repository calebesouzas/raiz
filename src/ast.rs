use crate::lexer::Token;

// Really wanted to use an arena instead... But the borrow checker is screaming at my ears.
// About borrowing 'self' as mutable many times in `crate::parser::Parser::parse_expr()`
pub enum Expr {
    Literal { value: i32 },
    Unary { target: Box<Expr> },
    Binary { left: Box<Expr>, right: Box<Expr>, op: Token },
    Void,
}

impl Expr {
    pub fn eval(&self) -> i32 {
        use Expr::*;
        match self {
            Literal {value} => *value,
            Unary {target} => -target.eval(),
            Binary {left, right, op} => {
                let l = left.eval();
                let r = right.eval();

                use Token::*;
                match op {
                    Plus => l + r,
                    Minus => l - r,
                    Star => l * r,
                    Slash => l / r,
                    other => panic!("not an operator: {:?}", other),
                }
            },
            Void => 0, // don't even know what to do about it...
        }
    }
}
