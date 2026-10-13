# syntax=docker/dockerfile:1

FROM oven/bun:1 AS web
WORKDIR /app/web
COPY web/package.json web/bun.lock ./
RUN bun install --frozen-lockfile
COPY web/ ./
RUN bun run build

FROM rust:bookworm AS builder
WORKDIR /app
COPY Cargo.toml Cargo.lock build.rs ./
COPY src ./src
COPY --from=web /app/web/dist ./web/dist
RUN cargo build --release

FROM debian:bookworm-slim
RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates wget \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /app/target/release/seasmoke /usr/local/bin/seasmoke
EXPOSE 3000
CMD ["seasmoke"]
