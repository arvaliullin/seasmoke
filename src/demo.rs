use crate::state::AppState;
use axum::{
    Json, Router,
    extract::{Path, State},
    http::StatusCode,
    routing::{get, post},
};
use serde::{Deserialize, Serialize};
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};
use tokio::sync::mpsc;

#[derive(Clone, Serialize)]
#[serde(rename_all = "lowercase")]
pub enum JobStatus {
    Running,
    Completed,
}

#[derive(Clone, Serialize)]
pub struct WorkerResult {
    pub id: u32,
    pub result: u64,
    pub elapsed_ms: u64,
}

#[derive(Clone, Serialize)]
pub struct Job {
    pub id: u64,
    pub status: JobStatus,
    pub worker_count: u32,
    pub workers: Vec<WorkerResult>,
    pub started_at: u64,
    pub finished_at: Option<u64>,
}

#[derive(Deserialize)]
pub struct FanoutRequest {
    #[serde(default = "default_workers")]
    pub workers: u32,
}

fn default_workers() -> u32 {
    8
}

#[derive(Serialize)]
pub struct FanoutResponse {
    pub job_id: u64,
}

fn now_unix_ms() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_millis() as u64)
        .unwrap_or(0)
}

async fn start_fanout(
    State(state): State<AppState>,
    Json(body): Json<FanoutRequest>,
) -> Json<FanoutResponse> {
    let workers = body.workers.clamp(1, 32);
    let job_id = state.next_job_id();
    let started_at = now_unix_ms();

    state.push_job(Job {
        id: job_id,
        status: JobStatus::Running,
        worker_count: workers,
        workers: Vec::new(),
        started_at,
        finished_at: None,
    });

    tracing::info!(job_id, workers, "fan-out job started");

    let state = state.clone();
    tokio::spawn(async move {
        let (tx, mut rx) = mpsc::channel::<WorkerResult>(workers as usize);

        for id in 0..workers {
            let tx = tx.clone();
            tokio::spawn(async move {
                let start = Instant::now();
                tracing::info!(job_id, worker = id, "worker started");

                // Deterministic delay in 50..=300ms (goroutine-style parallel work).
                let delay_ms = 50 + ((id.wrapping_mul(47)).wrapping_add(job_id as u32 * 13) % 251);
                tokio::time::sleep(Duration::from_millis(u64::from(delay_ms))).await;

                let result = u64::from(id) * u64::from(id);
                let elapsed_ms = start.elapsed().as_millis() as u64;
                tracing::info!(job_id, worker = id, result, elapsed_ms, "worker finished");

                let _ = tx
                    .send(WorkerResult {
                        id,
                        result,
                        elapsed_ms,
                    })
                    .await;
            });
        }
        drop(tx);

        let mut results = Vec::with_capacity(workers as usize);
        while let Some(result) = rx.recv().await {
            results.push(result);
        }
        results.sort_by_key(|worker| worker.id);

        let finished_at = now_unix_ms();
        state.finish_job(job_id, results, finished_at);
        tracing::info!(job_id, "fan-out job completed");
    });

    Json(FanoutResponse { job_id })
}

async fn list_jobs(State(state): State<AppState>) -> Json<Vec<Job>> {
    Json(state.list_jobs())
}

async fn get_job(
    State(state): State<AppState>,
    Path(id): Path<u64>,
) -> Result<Json<Job>, StatusCode> {
    state.get_job(id).map(Json).ok_or(StatusCode::NOT_FOUND)
}

pub fn router() -> Router<AppState> {
    Router::new()
        .route("/fanout", post(start_fanout))
        .route("/jobs", get(list_jobs))
        .route("/jobs/{id}", get(get_job))
}
