# Настраиваем PowerShell в качестве оболочки по умолчанию
set shell := ["powershell", "-Command"]

# Qt desktop client (desktop/)
qt_prefix := env_var_or_default("QT_PREFIX", "C:/Qt/6.11.2/mingw_64")
mingw_bin := env_var_or_default("MINGW_BIN", "C:/Qt/Tools/mingw1310_64/bin")
cmake_bin := env_var_or_default("CMAKE_BIN", "C:/Qt/Tools/CMake_64/bin")
ifw_root := env_var_or_default("IFW_ROOT", "C:/Qt/Tools/QtInstallerFramework/4.11")
qt_build_dir := "desktop/build/Debug"
qt_release_dir := "desktop/build/Release"
qt_target := "appseasmoke"
qt_exe := qt_build_dir + "/" + qt_target + ".exe"
qt_release_exe := qt_release_dir + "/" + qt_target + ".exe"
qt_package_id := "com.seasmoke.app"
qt_package_data := "desktop/installer/packages/" + qt_package_id + "/data"
qt_installer_config := "desktop/installer/config/config.xml"
qt_installer_packages := "desktop/installer/packages"
qt_dist_dir := "desktop/dist"
qt_installer_exe := qt_dist_dir + "/SeaSmoke-Setup-0.1.0.exe"
api_base := env_var_or_default("SEASMOKE_API_BASE", "http://seasmoke.local:3000")
compose_file := "deployments/docker-compose.yaml"

# Команда по умолчанию: web + Windows + Linux ARM64
default: build

# Сборка SPA (Vite)
build-web:
    Set-Location web; bun install; bun run build

# Нативная сборка под Windows (host)
build-windows: build-web
    cargo build --release

# Кросс-сборка под Linux ARM64 (Raspberry Pi 3) через Zig
build-linux-arm: build-web
    cargo zigbuild --target aarch64-unknown-linux-gnu --release

# Полная release-сборка: web, Windows и Linux ARM64
build: build-web
    cargo build --release
    cargo zigbuild --target aarch64-unknown-linux-gnu --release

# Собрать SPA и запустить бинарник (API + статика на :3000)
run: build-web
    cargo run --release

# Локальная разработка: Axum API на :3000
dev-backend:
    cargo run

# Локальная разработка: Vite на :5173 (proxy /api -> :3000)
dev-web:
    Set-Location web; bun install; bun run dev

# Qt: CMake configure (Debug)
configure-desktop:
    $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;{{cmake_bin}};" + $env:PATH; New-Item -ItemType Directory -Force -Path "{{qt_build_dir}}" | Out-Null; cmake -S desktop -B "{{qt_build_dir}}" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="{{qt_prefix}}" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="{{mingw_bin}}/g++.exe" -DCMAKE_C_COMPILER="{{mingw_bin}}/gcc.exe" -DCMAKE_MAKE_PROGRAM="{{mingw_bin}}/mingw32-make.exe"

# Qt: CMake configure (Release)
configure-desktop-release:
    $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;{{cmake_bin}};" + $env:PATH; New-Item -ItemType Directory -Force -Path "{{qt_release_dir}}" | Out-Null; cmake -S desktop -B "{{qt_release_dir}}" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="{{qt_prefix}}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="{{mingw_bin}}/g++.exe" -DCMAKE_C_COMPILER="{{mingw_bin}}/gcc.exe" -DCMAKE_MAKE_PROGRAM="{{mingw_bin}}/mingw32-make.exe"

# Qt: Debug build
build-desktop-debug:
    if (-not (Test-Path "{{qt_build_dir}}/CMakeCache.txt")) { just configure-desktop }; $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;{{cmake_bin}};" + $env:PATH; cmake --build "{{qt_build_dir}}" --target {{qt_target}}

# Qt: Release build
build-desktop:
    if (-not (Test-Path "{{qt_release_dir}}/CMakeCache.txt")) { just configure-desktop-release }; $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;{{cmake_bin}};" + $env:PATH; cmake --build "{{qt_release_dir}}" --target {{qt_target}}

# Qt desktop client -> seasmoke.local:3000
dev-desktop: build-desktop-debug
    $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;" + $env:PATH; $env:SEASMOKE_API_BASE = "{{api_base}}"; & "{{qt_exe}}"

# Qt: launch Debug without rebuilding
start-desktop:
    if (-not (Test-Path "{{qt_exe}}")) { just build-desktop-debug }; $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;" + $env:PATH; $env:SEASMOKE_API_BASE = "{{api_base}}"; & "{{qt_exe}}"

# Qt: stage Release app + runtime into IFW package data (windeployqt)
stage-desktop: build-desktop
    $env:PATH = "{{mingw_bin}};{{qt_prefix}}/bin;" + $env:PATH; if (Test-Path "{{qt_package_data}}") { Get-ChildItem "{{qt_package_data}}" -Force | Where-Object { $_.Name -ne '.gitkeep' } | Remove-Item -Recurse -Force }; New-Item -ItemType Directory -Force -Path "{{qt_package_data}}" | Out-Null; Copy-Item "{{qt_release_exe}}" "{{qt_package_data}}/{{qt_target}}.exe" -Force; windeployqt6 --release --compiler-runtime --qmldir desktop/qml "{{qt_package_data}}/{{qt_target}}.exe"

# Qt: offline installer via Qt Installer Framework
installer-desktop: stage-desktop
    $env:PATH = "{{ifw_root}}/bin;" + $env:PATH; New-Item -ItemType Directory -Force -Path "{{qt_dist_dir}}" | Out-Null; binarycreator --offline-only -c "{{qt_installer_config}}" -p "{{qt_installer_packages}}" "{{qt_installer_exe}}"; Write-Host "Installer: {{qt_installer_exe}}"

# Alias: full Release -> stage -> installer
dist-desktop: installer-desktop

# Залить Linux ARM64 бинарник на seasmoke
deploy: build-linux-arm
    scp target/aarch64-unknown-linux-gnu/release/seasmoke artur@seasmoke.local:~/

# Podman compose: поднять стек
up:
    podman compose -f {{compose_file}} up -d --build

# Podman compose: остановить стек
down:
    podman compose -f {{compose_file}} down

# Podman compose: остановить стек и удалить volumes
down-v:
    podman compose -f {{compose_file}} down -v

# Podman compose: статус
ps:
    podman compose -f {{compose_file}} ps --orphans=false

# Форматирование Rust
fmt:
    cargo fmt

# Линт: clippy + oxlint
lint:
    cargo clippy -- -D warnings
    Set-Location web; bun install; bun run lint

# Тесты
test:
    cargo test

# Очистка артефактов Cargo, SPA и Qt client
clean:
    cargo clean
    if (Test-Path web/dist) { Remove-Item -Recurse -Force web/dist }
    if (Test-Path desktop/build) { Remove-Item -Recurse -Force desktop/build }
    if (Test-Path desktop/dist) { Remove-Item -Recurse -Force desktop/dist }
    if (Test-Path "{{qt_package_data}}") { Get-ChildItem "{{qt_package_data}}" -Force | Where-Object { $_.Name -ne '.gitkeep' } | Remove-Item -Recurse -Force }
