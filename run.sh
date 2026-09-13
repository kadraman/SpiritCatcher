#!/usr/bin/env bash
# Run the built GBC ROM in Emulicious (discover env/PATH, else download to tools/emulicious).
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=scripts/lib/env.sh
source "$SCRIPT_DIR/scripts/lib/env.sh"

ROOT="$(spirit_repo_root)"
cd "$ROOT"

BUILD_TYPE="${BUILD_TYPE:-Debug}"
ROM=""
case "$BUILD_TYPE" in
  *Debug*) ROM="bin/gbc/rom_Debug.gbc" ;;
  *)       ROM="bin/gbc/rom.gbc" ;;
esac

if [[ ! -f "$ROM" ]]; then
  echo "error: ROM not found: $ROM" >&2
  echo "  Build first with: ./build.sh" >&2
  exit 1
fi

EMU="$(spirit_ensure_emulicious "$ROOT")"
export EMULICIOUS_PATH="$EMU"

# Avoid conflicting GTK themes on some Linux desktops.
if [[ "$(uname -s)" == "Linux" ]]; then
  unset GTK_PATH || true
fi

echo "Launching $ROM with $EMU"

case "$EMU" in
  *.jar)
    if ! command -v java >/dev/null 2>&1; then
      echo "error: Java is required to run Emulicious.jar" >&2
      exit 1
    fi
    exec java -jar "$EMU" "$ROM"
    ;;
  *)
    exec "$EMU" "$ROM"
    ;;
esac
