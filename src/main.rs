mod api;
mod demo;
mod request_log;
mod state;
mod static_files;

use axum::{Router, middleware};
use state::AppState;
use tokio::net::TcpListener;
use tracing_subscriber::EnvFilter;

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt()
        .with_env_filter(
            EnvFilter::try_from_default_env().unwrap_or_else(|_| EnvFilter::new("info")),
        )
        .init();

    let state = AppState::new();

    let app = Router::new()
        .nest("/api", api::router())
        .merge(static_files::router())
        .layer(middleware::from_fn_with_state(
            state.clone(),
            request_log::log_requests,
        ))
        .with_state(state);

    let listener = TcpListener::bind("0.0.0.0:3000")
        .await
        .expect("failed to bind 0.0.0.0:3000");

    tracing::info!("listening on http://0.0.0.0:3000");

    axum::serve(listener, app)
        .with_graceful_shutdown(shutdown_signal())
        .await
        .expect("server error");

    tracing::info!("shutdown complete");
}

async fn shutdown_signal() {
    let ctrl_c = async {
        tokio::signal::ctrl_c()
            .await
            .expect("failed to install Ctrl+C handler");
    };

    #[cfg(unix)]
    let terminate = async {
        tokio::signal::unix::signal(tokio::signal::unix::SignalKind::terminate())
            .expect("failed to install SIGTERM handler")
            .recv()
            .await;
    };

    #[cfg(not(unix))]
    let terminate = std::future::pending::<()>();

    tokio::select! {
        _ = ctrl_c => {},
        _ = terminate => {},
    }

    tracing::info!("shutdown signal received, finishing in-flight requests...");
}
