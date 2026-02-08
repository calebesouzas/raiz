use std::process::exit;

mod lexer;
use crate::lexer::Lexer;
use raiz_core::RaizProgram;

#[cfg(test)]
mod tests;

pub trait ProgramParser {
    fn build_ast(self) -> RaizProgram;
}

impl ProgramParser for RaizProgram {
    fn build_ast(mut self: RaizProgram) -> RaizProgram {
        let mut lexer = Lexer::new(&self.source);
        let lexer_result = lexer.tokenize();
        match lexer_result {
            Ok(token_vec) => {
                dbg!(&token_vec);
            }
            Err(e) => {
                eprintln!("{}Error\n\t{} at {}", e.kind, e.msg, e.pos.unwrap());
            }
        }

        self
    }
}
