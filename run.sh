#!/usr/bin/env bash
# Thin wrapper — implementation lives in scripts/run.sh
exec "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/scripts/run.sh" "$@"
