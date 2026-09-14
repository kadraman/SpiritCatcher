#!/usr/bin/env bash
# Bootstrap Spirit Catcher deps: CrossZGB submodule, GBDK, host tools, optional Emulicious.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/env.sh
source "$SCRIPT_DIR/lib/env.sh"

FORCE=0
WITH_EMULICIOUS=0
BUILD_TOOLS=1

usage() {
  cat <<'EOF'
Usage: scripts/bootstrap.sh [options]

  --force             Re-download GBDK even if present
  --with-emulicious   Download Emulicious into tools/emulicious if not found
  --skip-tools        Do not build CrossZGB host tools
  -h, --help          Show this help
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --force) FORCE=1 ;;
    --with-emulicious) WITH_EMULICIOUS=1 ;;
    --skip-tools) BUILD_TOOLS=0 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage >&2; exit 1 ;;
  esac
  shift
done

ROOT="$(spirit_repo_root)"
cd "$ROOT"
spirit_load_versions "$ROOT"

echo "==> Repo root: $ROOT"
echo "==> CrossZGB tag: $CROSSZGB_TAG"
echo "==> GBDK release: $GBDK_RELEASE"

# --- Submodule ---
if [[ ! -f "$ROOT/deps/CrossZGB/common/src/MakefileCommon" ]]; then
  echo "==> Initializing CrossZGB submodule..."
  git submodule update --init --recursive deps/CrossZGB
fi

echo "==> Checking out CrossZGB $CROSSZGB_TAG..."
git -C "$ROOT/deps/CrossZGB" fetch --tags --quiet
git -C "$ROOT/deps/CrossZGB" checkout -q "$CROSSZGB_TAG"

# --- Detect platform GBDK asset ---
uname_s="$(uname -s)"
uname_m="$(uname -m)"
case "$uname_s" in
  Linux)
    GBDK_ASSET="$GBDK_ASSET_LINUX_X64"
    GBDK_KIND=tar
    ;;
  Darwin)
    if [[ "$uname_m" == "arm64" ]]; then
      GBDK_ASSET="$GBDK_ASSET_MACOS_ARM64"
    else
      GBDK_ASSET="$GBDK_ASSET_MACOS_X64"
    fi
    GBDK_KIND=tar
    ;;
  MINGW*|MSYS*|CYGWIN*)
    GBDK_ASSET="$GBDK_ASSET_WINDOWS_X64"
    GBDK_KIND=zip
    ;;
  *)
    echo "error: unsupported OS: $uname_s" >&2
    exit 1
    ;;
esac

GBDK_URL="https://github.com/gbdk-2020/gbdk-2020/releases/download/${GBDK_RELEASE}/${GBDK_ASSET}"
GBDK_DIR="$ROOT/deps/gbdk"
LCC_BIN="$GBDK_DIR/bin/lcc"
LCC_EXE="$GBDK_DIR/bin/lcc.exe"

need_gbdk=1
if [[ $FORCE -eq 0 ]] && { [[ -x "$LCC_BIN" ]] || [[ -f "$LCC_EXE" ]]; }; then
  echo "==> GBDK already present at deps/gbdk (use --force to re-download)"
  need_gbdk=0
fi

if [[ $need_gbdk -eq 1 ]]; then
  echo "==> Downloading GBDK $GBDK_RELEASE ($GBDK_ASSET)..."
  tmp="$(mktemp -d "${TMPDIR:-/tmp}/spirit-gbdk.XXXXXX")"
  archive="$tmp/$GBDK_ASSET"
  if command -v curl >/dev/null 2>&1; then
    curl -fL --retry 3 -o "$archive" "$GBDK_URL"
  else
    wget -O "$archive" "$GBDK_URL"
  fi

  rm -rf "$GBDK_DIR"
  mkdir -p "$ROOT/deps"
  if [[ "$GBDK_KIND" == "tar" ]]; then
    tar -xzf "$archive" -C "$ROOT/deps"
  else
    unzip -o -q "$archive" -d "$ROOT/deps"
  fi
  rm -rf "$tmp"

  if [[ ! -d "$GBDK_DIR" ]]; then
    echo "error: expected $GBDK_DIR after extract" >&2
    exit 1
  fi
  echo "==> GBDK installed to deps/gbdk"
fi

# --- Build CrossZGB tools ---
if [[ $BUILD_TOOLS -eq 1 ]]; then
  if ! command -v make >/dev/null 2>&1; then
    echo "error: make is required to build CrossZGB tools" >&2
    exit 1
  fi
  echo "==> Building CrossZGB host tools..."
  tools_dir="$ROOT/deps/CrossZGB/tools"
  make_args=()
  if ! spirit_has_zlib; then
    echo "==> zlib.h not found; skipping vgm2psg (Spirit Catcher does not use VGM/PSG)"
    subdirs=""
    for d in "$tools_dir"/*/; do
      name="$(basename "$d")"
      [[ "$name" == "vgm2psg" ]] && continue
      subdirs+="$name/. "
    done
    make_args+=( "SUBDIRS=${subdirs% }" )
  fi
  # Bash <4.4 (macOS /bin/bash) errors on "${empty[@]}" with set -u
  make -C "$tools_dir" ${make_args[@]+"${make_args[@]}"}
fi

# --- Optional Emulicious ---
if [[ $WITH_EMULICIOUS -eq 1 ]]; then
  emu_path="$(spirit_ensure_local_emulicious "$ROOT" "$EMULICIOUS_URL")"
  echo "==> Emulicious.jar ready: $emu_path"
fi

echo "==> Bootstrap complete."
echo "    ZGB_PATH  -> $ROOT/deps/CrossZGB/common"
echo "    GBDK_HOME -> $ROOT/deps/gbdk"
echo "    Next: ./build.sh"
