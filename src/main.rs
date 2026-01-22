mod lexer;

use std::env;

fn main() {
    let version = "v0.0.1-test";
    let args = env::args();

    if args.len() == 1 {
        println!("Raiz language - {}", version);
    }

    for arg in args {
        println!("{}", arg);
    }
}
