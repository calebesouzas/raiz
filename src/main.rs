use std::io;


enum TokenType {
    NumberLiteral,
    BinaryOperator,
    Identifier,
    OpenParen,
    CloseParen,
    EOF
}

struct Token {
    kind: TokenType,
    value: String
}

fn tokenize(source_code: &String) {
    return;
}

fn main() {
    println!("Raiz - version 0.1-dev");
    println!("Use 'quit()' function to exit...");
    let mut code = String::new();
    loop {
        print!(">> ");
        io::stdin()
            .read_line(&mut code)
            .expect("Failed to read line");
        println!("{}", code);
    }
}
