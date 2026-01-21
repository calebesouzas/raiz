use std::env;

fn main() {
    println!("Raiz - version 0.1-dev");

    let args = env::args();

    for arg in args {
        println!("{}", arg);
    }
}
