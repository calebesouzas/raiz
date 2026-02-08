use std::{fmt, rc::Rc};

// Only 'line' and 'column' are printed.
// While 'len' can be used for other things.
// Like those '^^^'s and '---'s and even '+++'s
#[derive(Debug, PartialEq, Eq, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
    pub len: usize,
}
impl fmt::Display for Position {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "line {} column {}", self.line, self.column)
    }
}
impl Position {
    pub fn new() -> Self {
        Self {
            line: 1,
            column: 0, // will be 1 in the first char
            len: 1,
        }
    }
    pub fn copied(other: &Position) -> Position {
        Position {
            line: other.line,
            column: other.column,
            len: other.len,
        }
    }
    pub fn clone(&self) -> Position {
        Position {
            line: self.line,
            column: self.column,
            len: self.len,
        }
    }
    pub fn def_len(&mut self, new_len: usize) {
        self.len = new_len;
    }
    pub fn update(&mut self, ch: char) {
        if ch == '\n' {
            self.line += 1;
            self.column = 1;
            self.len = 1;
        } else {
            self.column += 1;
            self.len = 1;
        }
    }
}
pub struct RaizProgram {
    pub source: String,
    pub ast: Option<Vec<Stmt>>,
}

impl RaizProgram {
    pub fn new(source: String) -> Self {
        Self { source, ast: None }
    }
}

#[derive(Debug)]
pub struct Cursor<T> {
    vec: Vec<T>,
    current: usize,
    temp_current: usize,
}
impl<T> Cursor<T> {
    pub fn new(vec: Vec<T>) -> Self {
        Self {
            vec,
            current: 0,
            temp_current: 0,
        }
    }
    // returns the current item without consuming it
    pub fn current(&self) -> Option<&T> {
        self.vec.get(self.current)
    }
    // returns the next item consuming it
    pub fn next(&mut self) -> Option<&T> {
        self.advance();
        self.vec.get(self.current)
    }
    // retreats the cursor and returns the
    // previous item if there is one
    pub fn prev(&mut self) -> Option<&T> {
        self.retreat();
        self.vec.get(self.current)
    }

    pub fn advance(&mut self) {
        self.current += 1;
        self.temp_current = self.current;
    }

    pub fn retreat(&mut self) {
        self.current -= 1;
        self.temp_current = self.current;
    }
    // returns the next item without consuming it
    pub fn peek(&mut self) -> Option<&T> {
        self.temp_current += 1;
        self.vec.get(self.temp_current)
    }
    pub fn get_current(&self) -> usize {
        self.current
    }
    pub fn from_index(&self, index: usize) -> Option<&T> {
        self.vec.get(index)
    }
}

/* Yeah, we have so many token kinds...
 * And we'll probably have even more!
 * */
#[derive(Debug, PartialEq, Clone)] // why 'derive(Clone)'?
pub enum TokenKind {
    /* Operators */
    // Binary
    Plus,  // +
    Minus, // -
    Star,  // *
    Slash, // /

    // Bitwise
    RightShift, // >>
    LeftShift,  // <<
    Pipe,       // |
    Ampersand,  // &
    Tilde,      // ~
    Hat,        // ^

    // Booleans
    DoublePipe,  // ||
    DoubleAmper, // &&
    DoubleHat,   // ^^

    // Comparison
    LessThan,       // <
    LessOrEqual,    // <=
    GreaterThan,    // >
    GreaterOrEqual, // >=
    Equal,          // ==
    NotEqual,       // !=
    AlmostEqual,    // ~=

    // Arrows
    ThinArrow, // ->
    FatArrow,  // =>
    WaveArrow, // ~>
    CoolArrow, // :>

    // Grouping
    OpenParen,    // (
    CloseParen,   // )
    OpenBracket,  // [
    CloseBracket, // ]
    OpenBrace,    // {
    CloseBrace,   // }

    // Special symbols
    Exclamation,    // !
    Question,       // ?
    NullIdentifier, // _
    At,             // @
    HashTag,        // #
    Assign(bool),   // = (`bool` for 'inferred' if the previous char is ':')
    Collon,         // :
    DoubleCollon,   // ::

    // Literals
    NumberLiteral(i32),    // 123 e.g.
    CharLiteral(char),     // 'a' e.g.
    StringLiteral(String), // "Hello, world!" e.g.
    BooleanLiteral(bool),  // true/false

    Identifier(String), // variable e.g.

    // Keywords
    KeywordVar,     // var
    KeywordNot,     // not
    KeywordOr,      // or
    KeywordElse,    // else
    KeywordWhile,   // while
    KeywordFor,     // for
    KeywordTab,     // tab
    KeywordEnum,    // enum
    KeywordFun,     // fun
    KeywordOnEvent, // onevent

    EndOfFile,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Token {
    pub kind: TokenKind,
    pub pos: Position,
}
impl fmt::Display for Token {
    /* It is not the greatest implementation...
     * But it kinda works (i think, bruh).
     * It will just do a 'match' in the token
     * and return a string to a variable
     * just like the original text. */
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use TokenKind::*;
        let token_as_text = match &self.kind {
            // Binary
            Plus => "+",
            Minus => "-",
            Star => "*",
            Slash => "/",
            // Grouping
            OpenParen => "(",
            CloseParen => ")",
            OpenBracket => "[",
            CloseBracket => "]",
            OpenBrace => "{",
            CloseBrace => "}",
            // Bitwise
            RightShift => ">>",
            LeftShift => "<<",
            Pipe => "|",
            Ampersand => "&",
            Tilde => "~",
            Hat => "^",
            // Booleans
            DoublePipe => "||",
            DoubleAmper => "&&",
            DoubleHat => "^^",
            // Comparison
            LessThan => "<",
            GreaterThan => ">",
            LessOrEqual => "<=",
            GreaterOrEqual => ">=",
            Equal => "==",
            NotEqual => "!=",
            AlmostEqual => "~=",
            // Arrows
            ThinArrow => "->",
            FatArrow => "=>",
            WaveArrow => "~>",
            CoolArrow => ":>",
            // Special symbols
            Exclamation => "!",
            Question => "?",
            At => "@",
            HashTag => "#",
            Collon => ":",
            DoubleCollon => "::",
            NullIdentifier => "_",
            // Assignment
            Assign(inferred) => {
                if *inferred {
                    ":="
                } else {
                    "="
                }
            }
            // Literals
            NumberLiteral(num) => &num.to_string(),
            CharLiteral(c) => &c.to_string(),
            StringLiteral(string) => &string,
            BooleanLiteral(value) => {
                if *value {
                    "true"
                } else {
                    "false"
                }
            }
            Identifier(identifier) => &identifier,
            // Keywords
            KeywordVar => "var",
            KeywordNot => "not",
            KeywordOr => "or",
            KeywordElse => "else",
            KeywordWhile => "while",
            KeywordFor => "for",
            KeywordTab => "tab",
            KeywordEnum => "enum",
            KeywordFun => "fun",
            KeywordOnEvent => "onevent",

            EndOfFile => "end of file",
        };

        write!(f, "'{}' at {}", token_as_text, &self.pos)
    }
}

#[derive(Debug, PartialEq)]
// "Stmt" is an abreviation for "Statement"
// while "StmtKind" means "StatementKind"
// because let's be honest, writing
// `StatementKind::Variant`
// many times... Can be a little boring
pub struct Stmt {
    pub kind: StmtKind,
    pub pos: Position,
}

/* Never used `Rc` before
 * so i don't really know how to use it
 * (i'm a beginner programmer(?) by the way)
 * */
#[derive(Debug, PartialEq)]
pub enum StmtKind {
    SingleExpression(Expr),
    VarDeclaration {
        name: String,
        // @init
        // an optional expression
        // to give a value to the variable.
        // If not defined, it'll trigger
        // the 'default' event on the type,
        // if it's not handled, set to
        // `RaizObject::None` and send a
        // warning like "TYPE doesn't handle
        // the 'default' event".
        // If init == None, defined_type must
        // be Some(String).
        init: Option<Expr>,
        // @defined_type
        // the variable can have a
        // defined type using this syntax:
        // var defined: Type! // assignment is optional
        // // but will be the type's default.
        // // You can also use
        // var inferred := ...!
        // // where "..." is an obligatory expression
        // // after that, the type is on the variable,
        // // not only on the value anymore.
        // // And that type can't be changed
        defined_type: Option<String>,
    },
    FunDeclaration {
        name: String,
        params: Option<Vec<String>>,
        // @body
        // '[0].kind' must be `StmtKind::Block`
        // doesn't matter if it is empty
        // unless if defined_type != None
        body: Vec<Stmt>,
        // @defined_type
        // is the identifier after '->'
        // and right before the '{'
        defined_type: Option<String>,
    },
    Block(Option<Vec<Stmt>>),
    Return(Expr), // if used the 'return' keyword
}

#[derive(Debug, PartialEq)]
pub struct Expr {
    pub kind: ExprKind,
    pub pos: Position,
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
    Check(Box<Expr>),
    Call {
        callee: Box<Expr>,
        args: Vec<Expr>,
    },
}

#[derive(Debug, PartialEq)]
pub enum Operator {
    Sum,            // +
    Subtract,       // -
    Multiply,       // *
    Divide,         // /
    Dot,            // .
    Reference,      // & if unary
    LogicalNot,     // not
    LogicalAnd,     // &&
    LogicalOr,      // ||
    BitwiseNot,     // ~
    BitwiseAnd,     // &
    BitwiseOr,      // |
    BitwiseXor,     // ^
    Assign,         // =
    Equal,          // ==
    NotEqual,       // !=
    Greater,        // >
    GreaterOrEqual, // >=
    Less,           // <
    LessOrEqual,    // <=
}

#[derive(Debug, PartialEq)]
pub enum RaizObject {
    Int(i32),
    // Float(f32),
    Char(char),
    String(String),
    Function {
        // `Option<Vec<String>>`
        // for the names (or None if no params).
        // The assignment of the expressions in
        // the caller into the params
        // is done by the interpreter or compiler
        params: Option<Vec<String>>,
        body: Rc<Vec<Stmt>>,
    },
    EventHandler {
        name: String,
        params: Option<Vec<String>>,
        body: Rc<Vec<Stmt>>,
    },
    None,
}

pub struct RaizError {
    pub kind: String,
    pub msg: String,
    pub pos: Option<Position>,
}
