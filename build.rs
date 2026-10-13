use std::fs;
use std::path::Path;

fn main() {
    let dist = Path::new("web/dist");
    if !dist.exists() {
        fs::create_dir_all(dist).expect("create web/dist");
        fs::write(
            dist.join("index.html"),
            "<!DOCTYPE html><html><body>Build the web SPA first (just build-web).</body></html>\n",
        )
        .expect("write placeholder index.html");
    }

    println!("cargo:rerun-if-changed=web/dist");
}
