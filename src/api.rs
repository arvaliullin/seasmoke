use crate::demo;
use crate::request_log;
use crate::state::AppState;
use axum::{Json, Router, routing::get};
use serde::Serialize;

#[derive(Serialize)]
struct HealthResponse {
    status: &'static str,
}

async fn health() -> Json<HealthResponse> {
    Json(HealthResponse { status: "ok" })
}

pub fn router() -> Router<AppState> {
    Router::new()
        .route("/health", get(health))
        .route("/requests", get(request_log::list_requests))
        .nest("/demo", demo::router())
}
