use crate::state::AppState;
use axum::{
    Json,
    extract::{Request, State},
    middleware::Next,
    response::Response,
};
use serde::Serialize;
use std::time::{Instant, SystemTime, UNIX_EPOCH};

#[derive(Clone, Serialize)]
pub struct RequestEntry {
    pub method: String,
    pub path: String,
    pub status: u16,
    pub latency_ms: u64,
    pub at_unix_ms: u64,
}

fn now_unix_ms() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_millis() as u64)
        .unwrap_or(0)
}

pub async fn log_requests(State(state): State<AppState>, req: Request, next: Next) -> Response {
    let method = req.method().to_string();
    let path = req.uri().path().to_string();
    let is_api = path.starts_with("/api");
    let start = Instant::now();

    let response = next.run(req).await;

    if is_api {
        let status = response.status().as_u16();
        let latency_ms = start.elapsed().as_millis() as u64;
        tracing::info!(%method, %path, status, latency_ms, "request");
        state.push_request(RequestEntry {
            method,
            path,
            status,
            latency_ms,
            at_unix_ms: now_unix_ms(),
        });
    }

    response
}

pub async fn list_requests(State(state): State<AppState>) -> Json<Vec<RequestEntry>> {
    Json(state.list_requests())
}
