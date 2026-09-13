#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/env.sh
source "$SCRIPT_DIR/lib/env.sh"

ROOT="$(spirit_repo_root)"
spirit_apply_env "$ROOT"
spirit_require_deps "$ROOT"

BUILD_TYPE="${BUILD_TYPE:-Debug}"
# BUILD_TYPE=Release

cd "$ROOT/src"
make BUILD_TYPE="$BUILD_TYPE" gbc-clean
cd "$ROOT"
rm -rf "$ROOT/obj"
