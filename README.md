# seasmoke

Full-stack app: **Axum** API + **Vite / React / TypeScript** SPA (Gravity UI) + **Qt Quick** desktop client (Material).

## Run (API + SPA)

Собрать web и поднять сервер на `http://0.0.0.0:3000`:

```powershell
just run
```

## Development

Terminal 1 - backend (`http://0.0.0.0:3000`):

```powershell
just dev-backend
```

Terminal 2 - web SPA (`http://127.0.0.1:5173`, `/api` proxied to the backend):

```powershell
just dev-web
```

Terminal 2 (alt) - Qt desktop client (talks to `http://seasmoke.local:3000`):

```powershell
just dev-desktop
```

Requires [Bun](https://bun.sh/), a Rust toolchain, and for the Qt client: Qt 6. All recipes live in the root `justfile`.

Copy `.env.example` to `.env` if you need local overrides (`SEASMOKE_API_BASE`).

## Podman

Поднять API (+ embedded SPA) в контейнере:

```powershell
just up
```

Статус / остановка:

```powershell
just ps
just down
```

Compose file: `deployments/docker-compose.yaml`. Image build: `docker/seasmoke.Dockerfile`.

## Production build

```powershell
just build
```

This builds the SPA, then:

- Windows release binary: `target/release/seasmoke.exe`
- Linux ARM64 (Pi) via `cargo zigbuild`: `target/aarch64-unknown-linux-gnu/release/seasmoke`

Separate targets:

```powershell
just build-windows
just build-linux-arm
just build-web
just build-desktop
just installer-desktop
```

The release binary embeds `web/dist` and serves:

- `GET /api/health` - JSON health check
- SPA static assets + `index.html` fallback

The Qt client lives in `desktop/` and produces an offline installer via Qt Installer Framework (`just installer-desktop` -> `desktop/dist/`).

## Quality

```powershell
just fmt
just lint
just test
```

## Clean

```powershell
just clean
```
