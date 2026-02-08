use std::env;

use raiz_core::Program;
use raiz_parser::ProgramParser;

fn main() {
    let version = "v0.0.1-test";
    let args = env::args();

    // only the program name
    if args.len() == 1 {
        println!("Raiz language - {}", version);
    }

    // unexpected_char();

    var_dec_test();
}

fn unexpected_char() {
    let code = "ç".to_string();
    let mut p = Program::new(&code);
    p.build_ast();
}

fn var_dec_test() {
    let code = "var x = 10!".to_string();
    let mut p = Program::new(&code);

    p.build_ast();
}
