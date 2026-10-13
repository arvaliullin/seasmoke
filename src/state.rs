use crate::demo::Job;
use crate::request_log::RequestEntry;
use std::collections::VecDeque;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

const MAX_REQUESTS: usize = 100;
const MAX_JOBS: usize = 20;

#[derive(Clone)]
pub struct AppState {
    requests: Arc<Mutex<VecDeque<RequestEntry>>>,
    jobs: Arc<Mutex<VecDeque<Job>>>,
    next_job_id: Arc<AtomicU64>,
}

impl AppState {
    pub fn new() -> Self {
        Self {
            requests: Arc::new(Mutex::new(VecDeque::with_capacity(MAX_REQUESTS))),
            jobs: Arc::new(Mutex::new(VecDeque::with_capacity(MAX_JOBS))),
            next_job_id: Arc::new(AtomicU64::new(1)),
        }
    }

    pub fn push_request(&self, entry: RequestEntry) {
        let mut requests = self.requests.lock().expect("requests lock");
        if requests.len() >= MAX_REQUESTS {
            requests.pop_back();
        }
        requests.push_front(entry);
    }

    pub fn list_requests(&self) -> Vec<RequestEntry> {
        self.requests
            .lock()
            .expect("requests lock")
            .iter()
            .cloned()
            .collect()
    }

    pub fn next_job_id(&self) -> u64 {
        self.next_job_id.fetch_add(1, Ordering::Relaxed)
    }

    pub fn push_job(&self, job: Job) {
        let mut jobs = self.jobs.lock().expect("jobs lock");
        if jobs.len() >= MAX_JOBS {
            jobs.pop_back();
        }
        jobs.push_front(job);
    }

    pub fn finish_job(
        &self,
        job_id: u64,
        workers: Vec<crate::demo::WorkerResult>,
        finished_at: u64,
    ) {
        let mut jobs = self.jobs.lock().expect("jobs lock");
        if let Some(job) = jobs.iter_mut().find(|job| job.id == job_id) {
            job.status = crate::demo::JobStatus::Completed;
            job.workers = workers;
            job.finished_at = Some(finished_at);
        }
    }

    pub fn list_jobs(&self) -> Vec<Job> {
        self.jobs
            .lock()
            .expect("jobs lock")
            .iter()
            .cloned()
            .collect()
    }

    pub fn get_job(&self, id: u64) -> Option<Job> {
        self.jobs
            .lock()
            .expect("jobs lock")
            .iter()
            .find(|job| job.id == id)
            .cloned()
    }
}
