//use std::process::exit;

mod lexer;
use crate::lexer::Lexer;
use raiz_core::Program;

mod parser;

#[cfg(test)]
mod tests;

pub trait ProgramParser {
    fn build_ast(self) -> Program;
}

impl ProgramParser for Program {
    fn build_ast(mut self: Program) -> Program {
        let mut lexer = Lexer::new(&self.source);
        let lexer_result = lexer.tokenize();
        match lexer_result {
            Ok(token_vec) => {
                dbg!(&token_vec);
            }
            Err(e) => {
                eprintln!("Error: {}", e.msg);
            }
        }

        self
    }
}
