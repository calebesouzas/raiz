use std::{fmt, rc::Rc};

pub struct Program {
    pub source: String,
    pub code: Option<Rc<Vec<Stmt>>>,
}
impl Program {
    fn new(source: &String) -> Self {
        Self {
            source: source.to_string(),
            code: None,
        }
    }
}

pub struct Cursor<T> {
    vec: Vec<T>,
    index: usize,
}
impl<T> Cursor<T> {
    pub fn new(vec: Vec<T>) -> Self {
        Self { vec, index: 0 }
    }
    pub fn current(&self) -> Option<&T> {
        self.vec.get(self.index)
    }
    pub fn next(&self) -> Option<&T> {
        self.vec.get(self.index + 1)
    }
    pub fn prev(&self) -> Option<&T> {
        self.vec.get(self.index - 1)
    }
    pub fn advance(&mut self) {
        self.index += 1;
    }
    pub fn retreat(&mut self) {
        self.index -= 1;
    }
    pub fn consume(&mut self) -> Option<&T> {
        // Needed to do something crazy here
        // first we update the index and
        // then we pick the previous item
        // We need to do this due to some
        // borrowing and mutability issues
        self.advance();
        let current = self.prev();
        current
    }
}

#[derive(Debug, PartialEq, Eq, Clone)]
pub struct Position {
    line: usize,
    column: usize,
    length: usize,
}
impl fmt::Display for Position {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "line {} column {}", self.line, self.column)
    }
}

#[derive(Debug, PartialEq, Clone)]
pub struct Token {
    kind: TokenKind,
    pos: Position,
}
impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let token_as_text = match &self.kind {
            TokenKind::Plus => "+",
            TokenKind::Minus => "-",
            TokenKind::Star => "*",
            TokenKind::Slash => "/",
            TokenKind::OpenParen => "(",
            TokenKind::CloseParen => ")",
            TokenKind::OpenBracket => "[",
            TokenKind::CloseBracket => "]",
            TokenKind::OpenBrace => "{",
            TokenKind::CloseBrace => "}",
            TokenKind::Exclamation => "!",
            TokenKind::Question => "?",
            TokenKind::NumberLiteral(num) => &num.to_string(),
            TokenKind::FloatLiteral(num) => &num.to_string(),
            TokenKind::CharLiteral(c) => &c.to_string(),
            TokenKind::StringLiteral(string) => &string,
            TokenKind::Identifier(identifier) => &identifier,
            TokenKind::KeywordVar => "var",
            TokenKind::KeywordOr => "or",
            TokenKind::KeywordElse => "else",
            TokenKind::EndOfFile => "end of file",
        };

        write!(f, "'{}'", token_as_text)
    }
}

#[derive(Debug, PartialEq, Clone)]
pub enum TokenKind {
    Plus,
    Minus,
    Star,
    Slash,
    OpenParen,
    CloseParen,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    Exclamation,
    Question,
    NumberLiteral(i32),
    FloatLiteral(f32),
    CharLiteral(char),
    StringLiteral(String),
    Identifier(String),
    KeywordVar,
    KeywordOr,
    KeywordElse,
    EndOfFile,
}

#[derive(Debug, PartialEq)]
pub struct Stmt {
    kind: StmtKind,
    pos: Position,
}

#[derive(Debug, PartialEq)]
pub enum StmtKind {
    Expression(Expr),
    VarDeclaration { name: String, init_expr: Expr },
    Print(Expr),
}

#[derive(Debug, PartialEq)]
pub struct Expr {
    kind: ExprKind,
    pos: Position,
}

#[derive(Debug, PartialEq)]
pub enum ExprKind {
    Literal(RaizObject),
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
}

#[derive(Debug, PartialEq)]
pub enum Operator {
    Sum,
    Subtract,
    Multiply,
    Divide,
    Unary,
    // And others like Dot, Collon and more...
}

#[derive(Debug, PartialEq)]
pub enum RaizObject {
    Int(i32),
    Float(f32),
    Char(char),
    String(String),
    None,
}
