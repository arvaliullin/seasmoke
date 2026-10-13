use crate::state::AppState;
use axum::{
    Router,
    body::Body,
    extract::Request,
    http::{StatusCode, header, uri::Uri},
    response::{IntoResponse, Response},
    routing::get,
};
use mime_guess::from_path;
use rust_embed::Embed;

#[derive(Embed)]
#[folder = "web/dist"]
struct Assets;

async fn static_handler(uri: Uri) -> impl IntoResponse {
    let path = uri.path().trim_start_matches('/');
    let path = if path.is_empty() { "index.html" } else { path };

    match Assets::get(path) {
        Some(file) => {
            let mime = from_path(path).first_or_octet_stream();
            Response::builder()
                .status(StatusCode::OK)
                .header(header::CONTENT_TYPE, mime.as_ref())
                .body(Body::from(file.data.to_vec()))
                .unwrap()
        }
        None => match Assets::get("index.html") {
            Some(index) => Response::builder()
                .status(StatusCode::OK)
                .header(header::CONTENT_TYPE, "text/html; charset=utf-8")
                .body(Body::from(index.data.to_vec()))
                .unwrap(),
            None => Response::builder()
                .status(StatusCode::NOT_FOUND)
                .body(Body::from("Not Found"))
                .unwrap(),
        },
    }
}

async fn fallback_handler(req: Request) -> impl IntoResponse {
    let uri = req.uri().clone();
    static_handler(uri).await
}

pub fn router() -> Router<AppState> {
    Router::new()
        .route("/", get(static_handler))
        .fallback(fallback_handler)
}
