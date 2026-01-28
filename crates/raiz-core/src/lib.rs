#[derive(Debug, PartialEq, Eq, Clone)]
pub struct Position {
    line: usize,
    column: usize,
    length: usize,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Token {
    kind: TokenKind,
    pos: Position,
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
