import {Moon, Sun} from '@gravity-ui/icons';
import {
  Button,
  Flex,
  Icon,
  Text,
  type Theme,
  useThemeValue,
} from '@gravity-ui/uikit';
import {useCallback, useEffect, useState} from 'react';

import './App.css';

interface AppProps {
  theme: Theme;
  onThemeChange: (theme: Theme) => void;
}

interface RequestEntry {
  method: string;
  path: string;
  status: number;
  latency_ms: number;
  at_unix_ms: number;
}

interface WorkerResult {
  id: number;
  result: number;
  elapsed_ms: number;
}

interface Job {
  id: number;
  status: 'running' | 'completed';
  worker_count: number;
  workers: WorkerResult[];
  started_at: number;
  finished_at: number | null;
}

function App({theme, onThemeChange}: AppProps) {
  const themeValue = useThemeValue();
  const [health, setHealth] = useState<string>('loading...');
  const [requests, setRequests] = useState<RequestEntry[]>([]);
  const [jobs, setJobs] = useState<Job[]>([]);
  const [starting, setStarting] = useState(false);

  useEffect(() => {
    let cancelled = false;

    fetch('/api/health')
      .then(async (response) => {
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        const data = (await response.json()) as {status?: string};
        if (!cancelled) {
          setHealth(data.status ?? 'unknown');
        }
      })
      .catch((error: unknown) => {
        if (!cancelled) {
          const message = error instanceof Error ? error.message : 'unavailable';
          setHealth(message);
        }
      });

    return () => {
      cancelled = true;
    };
  }, []);

  const refreshRequests = useCallback(async () => {
    try {
      const response = await fetch('/api/requests');
      if (!response.ok) {
        return;
      }
      const data = (await response.json()) as RequestEntry[];
      setRequests(data);
    } catch {
      // keep previous snapshot
    }
  }, []);

  const refreshJobs = useCallback(async () => {
    try {
      const response = await fetch('/api/demo/jobs');
      if (!response.ok) {
        return;
      }
      const data = (await response.json()) as Job[];
      setJobs(data);
    } catch {
      // keep previous snapshot
    }
  }, []);

  useEffect(() => {
    void refreshRequests();
    void refreshJobs();
    const timer = window.setInterval(() => {
      void refreshRequests();
      void refreshJobs();
    }, 2000);
    return () => window.clearInterval(timer);
  }, [refreshJobs, refreshRequests]);

  const runFanout = async () => {
    setStarting(true);
    try {
      const response = await fetch('/api/demo/fanout', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({workers: 8}),
      });
      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`);
      }
      await refreshJobs();
      await refreshRequests();
    } catch {
      // next poll will catch up
    } finally {
      setStarting(false);
    }
  };

  const nextTheme: Theme = themeValue.startsWith('dark') ? 'light' : 'dark';

  return (
    <Flex className="app" direction="column" gap={5} alignItems="center">
      <Text variant="header-1">SeaSmoke</Text>
      <Text variant="body-1" color="secondary">
        Full-stack scaffold: Axum API + Vite SPA
      </Text>
      <Flex gap={3} alignItems="center">
        <Button
          view="outlined"
          size="l"
          onClick={() => onThemeChange(nextTheme)}
        >
          <Icon data={themeValue.startsWith('dark') ? Sun : Moon} size={16} />
          Theme: {themeValue}
        </Button>
        <Button
          view="flat"
          size="l"
          onClick={() => onThemeChange('system')}
          disabled={theme === 'system'}
        >
          System
        </Button>
      </Flex>
      <Text variant="body-2">
        API health: <Text as="span" color="complementary">{health}</Text>
      </Text>

      <Flex className="section" direction="column" gap={3} alignItems="stretch">
        <Text variant="header-2">Fan-out demo</Text>
        <Text variant="body-2" color="secondary">
          tokio::spawn workers (Go goroutine analogue): fan-out / fan-in
        </Text>
        <Button view="action" size="l" loading={starting} onClick={() => void runFanout()}>
          Run 8 workers
        </Button>
        {jobs.length === 0 ? (
          <Text variant="body-2" color="secondary">
            No jobs yet
          </Text>
        ) : (
          <Flex direction="column" gap={3}>
            {jobs.map((job) => (
              <Flex key={job.id} direction="column" gap={1} className="job">
                <Text variant="body-2">
                  Job #{job.id} - {job.status} ({job.worker_count} workers)
                </Text>
                {job.workers.length > 0 ? (
                  <Flex direction="column" gap={1} className="mono">
                    {job.workers.map((worker) => (
                      <Text key={worker.id} variant="code-1">
                        worker {worker.id}: result={worker.result} {worker.elapsed_ms}ms
                      </Text>
                    ))}
                  </Flex>
                ) : (
                  <Text variant="body-2" color="secondary">
                    running...
                  </Text>
                )}
              </Flex>
            ))}
          </Flex>
        )}
      </Flex>

      <Flex className="section" direction="column" gap={3} alignItems="stretch">
        <Text variant="header-2">Recent requests</Text>
        <Text variant="body-2" color="secondary">
          Logged by Axum middleware (API routes only)
        </Text>
        {requests.length === 0 ? (
          <Text variant="body-2" color="secondary">
            No API requests yet
          </Text>
        ) : (
          <Flex direction="column" gap={1} className="mono">
            {requests.slice(0, 20).map((entry, index) => (
              <Text key={`${entry.at_unix_ms}-${entry.path}-${index}`} variant="code-1">
                {entry.method} {entry.path} {entry.status} {entry.latency_ms}ms
              </Text>
            ))}
          </Flex>
        )}
      </Flex>
    </Flex>
  );
}

export default App;
