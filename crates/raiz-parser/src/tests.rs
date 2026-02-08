use crate::ProgramParser;
use raiz_core::RaizProgram;

#[test]
fn invalid_character() {
    let code = "ç".to_string();
    let mut p = RaizProgram::new(code);
    p.build_ast();
}

#[test]
fn print_tokens_vec() {
    let code = "var num = 15!".to_string();

    let mut p = RaizProgram::new(code);
    p.build_ast();
}
