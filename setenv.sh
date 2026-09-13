#!/usr/bin/env bash
# Export project-local CrossZGB / GBDK / Emulicious environment.
# Usage: source ./setenv.sh

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=scripts/lib/env.sh
source "$SCRIPT_DIR/scripts/lib/env.sh"

ROOT="$(spirit_repo_root)"
spirit_apply_env "$ROOT"

if [[ ! -f "$ZGB_PATH/src/MakefileCommon" ]]; then
  echo "warning: CrossZGB submodule missing; run: git submodule update --init --recursive" >&2
fi
if [[ ! -x "$GBDK_HOME/bin/lcc" && ! -f "$GBDK_HOME/bin/lcc.exe" ]]; then
  echo "warning: GBDK missing; run: scripts/bootstrap.sh" >&2
fi

echo "ZGB_PATH=$ZGB_PATH"
echo "GBDK_HOME=$GBDK_HOME"
if [[ -n "${EMULICIOUS_PATH:-}" ]]; then
  echo "EMULICIOUS_PATH=$EMULICIOUS_PATH"
fi
