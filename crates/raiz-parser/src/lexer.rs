use raiz_core::{Cursor, Error, Position, Token};

pub struct Lexer {
    source: String,
    cursor: Cursor<char>,
    tokens: Vec<Token>,
    pos: Position,
}

impl Lexer {
    pub fn new(code: &String) -> Self {
        let source = code.to_owned();
        let cursor = Cursor::new(source.chars().collect());
        let tokens = Vec::<Token>::new();
        let pos = Position::new();
        Self {
            source,
            cursor,
            tokens,
            pos,
        }
    }
    pub fn tokenize(&mut self) -> Result<Vec<Token>, Error> {
        let mut tokens = Vec::<Token>::new();
        while let Some(c) = self.cursor.current() {
            let current_pos = self.pos.clone();
            // I do it because i can't return from the function
            // without assigning a `TokenKind` value to `token_kind`
            let mut error: Option<Error> = None;
            use Token::*;
            let token_kind = match c {
                '+' => Plus,
                '-' => Minus,
                // don't forget to skip comments (but updating position)!
                // single line with '//' and multi-line with '/**/'
                '*' => Star,
                '/' => {
                    if let Some('/') = self.cursor.peek() {
                        self.cursor.advance(); // consume current '/'
                        self.cursor.advance(); // consume next '/'
                        let mut len: usize = 1;
                        while let Some(ch) = self.cursor.peek()
                            && *ch != '\n'
                        {
                            len += 1;
                            self.cursor.advance();
                        }
                        self.pos.column += len;
                        self.pos.line += 1;
                        EndOfFile // it won't be added to the vec
                    } else {
                        Slash
                    }
                }
                '(' => OpenParen,
                ')' => CloseParen,
                '[' => OpenBracket,
                ']' => CloseBracket,
                '{' => OpenBrace,
                '}' => CloseBrace,
                '!' => {
                    if let Some('=') = self.peek() {
                        self.advance();
                        NotEqual
                    } else {
                        Exclamation
                    }
                }
                '?' => Question,
                '@' => At,
                '#' => HashTag,
                ':' => {
                    if let Some(ch) = self.peek() {
                        let result = match ch {
                            '>' => CoolArrow,
                            ':' => DoubleCollon,
                            _ => Collon,
                        };
                        self.advance(); // consume ch
                        result
                    } else {
                        Collon
                    }
                }
                '=' => {
                    if let Some(ch) = self.peek() {
                        let result = match ch {
                            '=' => Equal,
                            '>' => FatArrow,
                            _ => Assign,
                        };
                        self.advance();
                        result
                    } else {
                        Assign
                    }
                }
                // handling number literals, no support for floating
                // numbers at the moment... But it shouldn't be hard
                // to implement... By the way, '_'s are allowed for
                // better readability of the number.
                '0'..='9' => {
                    let mut len: usize = 1;
                    let mut number = 0;
                    while let Some(ch) = self.current()
                        && (ch.is_ascii_digit() || *ch == '_')
                    {
                        if *ch != '_' {
                            number = number * 10 + (*ch as i32 - 48);
                        }
                        self.advance();
                        len += 1;
                    }
                    self.cursor.retreat();
                    // ^- this prevents skipping the chararacter
                    // right after the last one in the number
                    self.pos.def_len(len);
                    NumberLiteral(number)
                }

                // handling identifiers. They need to start with a $
                // (optionaly, but obligatory if using a keyword as an
                // identifier is needed) or any ASCII alphabhetic character
                // the following characters can be alpha-numeric and '_'
                '$' | 'a'..='z' | 'A'..='Z' => {
                    let start: usize = self.cursor.get_current();
                    let mut end = start;
                    while let Some(ch) = self.current()
                        && (ch.is_ascii_alphanumeric() || *ch == '_')
                    {
                        self.advance();
                        end += 1;
                    }
                    self.cursor.retreat();
                    let len = end - start;
                    self.pos.def_len(len);
                    let identifier = &self.source[start..end];
                    match identifier {
                        "var" => KeywordVar,
                        "not" => KeywordNot,
                        "or" => KeywordOr,
                        "else" => KeywordElse,
                        "while" => KeywordWhile,
                        "for" => KeywordFor,
                        "tab" => KeywordTab,
                        "enum" => KeywordEnum,
                        "fun" => KeywordFun,
                        "onevent" => KeywordOnEvent,
                        _ => Identifier(identifier.to_owned()),
                    }
                }
                // '_' alone only, not when in the middle of an identifier
                '_' => NullIdentifier,
                // handling character literal
                '\'' => {
                    if let Some(char_literal) = self.peek() {
                        let result = match char_literal {
                            '\t' | '\n' | '\r' => {
                                let msg = "Use escape sequence instead";
                                error = Some(Error {
                                    msg: msg.to_string(),
                                    pos: None,
                                });
                                EndOfFile
                            }
                            '\'' => {
                                error = Some(Error {
                                    msg: "Empty character literal".to_string(),
                                    pos: Some(self.pos.clone()),
                                });
                                EndOfFile
                            }
                            other => CharLiteral(*other),
                        };
                        result
                    } else {
                        error = Some(Error {
                            msg: "Not closed character literal".to_string(),
                            pos: None,
                        });
                        EndOfFile
                    }
                }
                other => {
                    if !other.is_ascii_whitespace() {
                        error = Some(Error {
                            msg: format!("Unexpected character: > {} <", other),
                            pos: None,
                        });
                    }
                    EndOfFile
                }
            };
            self.advance();
            let token_pos = current_pos;
            let token = token_kind;
            if let Some(mut e) = error {
                e.pos = Some(self.pos.clone());
                return Err(e);
            }
            if token != EndOfFile {
                tokens.push(token);
            }
        }
        let mut final_pos = self.pos.clone();
        final_pos.column += 1; // to be located right after the last character
        tokens.push(Token::EndOfFile);
        Ok(tokens)
    }
    fn current(&self) -> Option<&char> {
        self.cursor.current()
    }
    fn peek(&self) -> Option<&char> {
        self.cursor.peek()
    }
    fn next(&mut self) -> Option<&char> {
        self.cursor.next()
    }
    fn advance(&mut self) {
        self.cursor.advance();
    }
}
