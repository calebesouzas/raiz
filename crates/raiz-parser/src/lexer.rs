use raiz_core::{Cursor, Position, RaizError, Token, TokenKind};

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
    pub fn tokenize(&mut self) -> Result<Vec<Token>, RaizError> {
        let mut tokens = Vec::<Token>::new();
        while let Some(c) = self.cursor.next() {
            // will automaticaly update the lines and columns
            // based on what character it is, doesn't matter
            // if it is a valid character or not in it's context
            self.pos.update(*c);
            // I do it because i can't return from the function
            // without assigning a `TokenKind` value to `token_kind`
            let mut error: Option<RaizError> = None;
            use TokenKind::*;
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
                    if let Some('=') = self.cursor.peek() {
                        self.cursor.advance();
                        NotEqual
                    } else {
                        Exclamation
                    }
                }
                '?' => Question,
                '@' => At,
                '#' => HashTag,
                ':' => {
                    if let Some(ch) = self.cursor.peek() {
                        self.cursor.advance(); // consume ch
                        match ch {
                            '>' => CoolArrow,
                            '=' => Assign(true),
                            ':' => DoubleCollon,
                            _ => Collon,
                        }
                    } else {
                        Collon
                    }
                }
                '=' => {
                    if let Some(ch) = self.cursor.peek() {
                        self.cursor.advance();
                        match ch {
                            '=' => Equal,
                            '>' => FatArrow,
                            _ => Assign(false),
                        }
                    } else {
                        Assign(false)
                    }
                }
                // handling number literals, no support for floating
                // numbers at the moment... But it shouldn't be hard
                // to implement... By the way, '_'s are allowed for
                // better readability of the number.
                '0'..='9' => {
                    let mut len: usize = 1;
                    let mut number = 0;
                    while let Some(ch) = self.cursor.peek()
                        && (ch.is_ascii_digit() || *ch == '_')
                    {
                        self.cursor.advance(); // consume ch
                        if *ch != '_' {
                            number = number * 10 + (*ch as i32 - 48);
                        }
                        len += 1;
                    }
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
                    while let Some(ch) = self.cursor.peek()
                        && (ch.is_ascii_alphanumeric() || *ch == '_')
                    {
                        end += 1;
                        self.cursor.advance();
                    }
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
                    if let Some(next_c) = self.cursor.next() {
                        if let Some('\'') = self.cursor.peek() {
                            self.cursor.advance(); // consume '\''
                            match next_c {
                                '\t' | '\n' | '\r' | '\\' => {
                                    let msg = "Use escape sequence instead";
                                    error = Some(RaizError {
                                        kind: "Lexical".to_string(),
                                        msg: msg.to_string(),
                                        pos: None,
                                    })
                                }
                                _ => (),
                            }
                            CharLiteral(*next_c)
                        } else {
                            error = Some(RaizError {
                                kind: "Lexical".to_string(),
                                msg: "Invalid character literal".to_string(),
                                pos: None,
                            });
                            EndOfFile
                        }
                    } else {
                        error = Some(RaizError {
                            kind: "Lexical".to_string(),
                            msg: "Not closed character literal".to_string(),
                            pos: None,
                        });
                        EndOfFile
                    }
                }
                other => {
                    if !other.is_ascii_whitespace() {
                        error = Some(RaizError {
                            kind: "Lexical".to_string(),
                            msg: format!("Unexpected character: > {} <", other),
                            pos: None,
                        });
                    }
                    EndOfFile
                }
            };

            let token_pos = self.pos.clone();
            let token = Token {
                kind: token_kind,
                pos: token_pos,
            };
            if let Some(mut e) = error {
                e.pos = Some(self.pos.clone());
                return Err(e);
            }
            if token.kind != TokenKind::EndOfFile {
                tokens.push(token);
            }
        }
        let mut final_pos = self.pos.clone();
        final_pos.column += 1; // to be located right after the last character
        tokens.push(Token {
            kind: TokenKind::EndOfFile,
            pos: final_pos,
        });
        Ok(tokens)
    }
}
