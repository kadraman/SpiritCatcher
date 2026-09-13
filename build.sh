#!/usr/bin/env bash
# Thin wrapper — implementation lives in scripts/build.sh
exec "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/scripts/build.sh" "$@"
