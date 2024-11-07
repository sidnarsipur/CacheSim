fn main() {
    println!("cargo:rerun-if-changed=src/simulator.c");
    cc::Build::new()
        .file("src/simulator.c")
        .compile("simulator");
}
