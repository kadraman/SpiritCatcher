#!/usr/bin/env bash
# Make sure tools/emulicious/Emulicious.jar exists for F5 / Emulicious Debugger.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/env.sh
source "$SCRIPT_DIR/lib/env.sh"

ROOT="$(spirit_repo_root)"
spirit_load_versions "$ROOT"
jar="$(spirit_ensure_local_emulicious "$ROOT" "$EMULICIOUS_URL")"
echo "Emulicious.jar: $jar"
