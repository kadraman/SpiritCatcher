#!/usr/bin/env bash
# Shared path / dependency helpers for Spirit Catcher (bash).
# Sourced by setenv.sh, bootstrap.sh, and run.sh.

spirit_repo_root() {
  local here
  here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  # scripts/lib -> repo root
  if [[ -f "$here/../../deps/VERSIONS" ]]; then
    cd "$here/../.." && pwd
  elif [[ -f "$here/../deps/VERSIONS" ]]; then
    cd "$here/.." && pwd
  elif [[ -f "$here/deps/VERSIONS" ]]; then
    cd "$here" && pwd
  else
    # Fallback: walk up looking for deps/VERSIONS
    local dir="$here"
    while [[ "$dir" != "/" ]]; do
      if [[ -f "$dir/deps/VERSIONS" ]]; then
        echo "$dir"
        return 0
      fi
      dir="$(dirname "$dir")"
    done
    echo "error: could not locate Spirit Catcher repo root (deps/VERSIONS)" >&2
    return 1
  fi
}

spirit_load_versions() {
  local root="$1"
  local line key value
  while IFS= read -r line || [[ -n "$line" ]]; do
    line="${line%$'\r'}"
    [[ "$line" =~ ^[A-Z0-9_]+= ]] || continue
    key="${line%%=*}"
    value="${line#*=}"
    export "$key=$value"
  done < "$root/deps/VERSIONS"
}

spirit_require_deps() {
  local root="$1"
  if [[ ! -f "$root/deps/CrossZGB/common/src/MakefileCommon" ]]; then
    echo "error: CrossZGB submodule missing at deps/CrossZGB" >&2
    echo "  Run: git submodule update --init --recursive" >&2
    echo "  Then: scripts/bootstrap.sh" >&2
    return 1
  fi
  if [[ ! -x "$root/deps/gbdk/bin/lcc" && ! -f "$root/deps/gbdk/bin/lcc.exe" ]]; then
    echo "error: GBDK not found at deps/gbdk" >&2
    echo "  Run: scripts/bootstrap.sh" >&2
    return 1
  fi
}

# Resolve Emulicious to a path (jar, exe, or directory). Prints path; returns 0 if found.
# Does not download. Set SPIRIT_SKIP_DOWNLOAD=1 behavior is in ensure helpers.
spirit_find_emulicious() {
  local root="${1:-}"
  local candidate

  if [[ -n "${EMULICIOUS_PATH:-}" ]]; then
    candidate="$EMULICIOUS_PATH"
    if [[ -d "$candidate" ]]; then
      if [[ -f "$candidate/Emulicious.jar" ]]; then
        echo "$candidate/Emulicious.jar"
        return 0
      fi
      if [[ -f "$candidate/Emulicious.exe" ]]; then
        echo "$candidate/Emulicious.exe"
        return 0
      fi
    elif [[ -f "$candidate" ]]; then
      echo "$candidate"
      return 0
    fi
  fi

  for name in emulicious Emulicious Emulicious.jar; do
    if command -v "$name" >/dev/null 2>&1; then
      command -v "$name"
      return 0
    fi
  done

  local uname_s
  uname_s="$(uname -s 2>/dev/null || echo unknown)"
  case "$uname_s" in
    Darwin)
      for candidate in \
        "/Applications/Emulicious/Emulicious.jar" \
        "$HOME/Applications/Emulicious/Emulicious.jar" \
        "$HOME/Emulicious/Emulicious.jar"
      do
        if [[ -f "$candidate" ]]; then
          echo "$candidate"
          return 0
        fi
      done
      ;;
    Linux|*)
      for candidate in \
        "$HOME/Emulicious/Emulicious.jar" \
        "$HOME/.local/share/Emulicious/Emulicious.jar" \
        "/opt/Emulicious/Emulicious.jar" \
        "/usr/local/share/Emulicious/Emulicious.jar"
      do
        if [[ -f "$candidate" ]]; then
          echo "$candidate"
          return 0
        fi
      done
      ;;
  esac

  if [[ -n "$root" ]]; then
    for candidate in \
      "$root/tools/emulicious/Emulicious.jar" \
      "$root/tools/emulicious/Emulicious.exe"
    do
      if [[ -f "$candidate" ]]; then
        echo "$candidate"
        return 0
      fi
    done
  fi

  return 1
}

spirit_download_emulicious() {
  local root="$1"
  local url="${2:-}"
  local dest="$root/tools/emulicious"
  local tmp zipfile

  if [[ -z "$url" ]]; then
    spirit_load_versions "$root"
    url="$EMULICIOUS_URL"
  fi

  mkdir -p "$dest"
  tmp="$(mktemp -d "${TMPDIR:-/tmp}/spirit-emulicious.XXXXXX")"
  zipfile="$tmp/emulicious.zip"

  echo "Downloading Emulicious to $dest ..."
  if command -v curl >/dev/null 2>&1; then
    curl -fL --retry 3 -o "$zipfile" "$url"
  elif command -v wget >/dev/null 2>&1; then
    wget -O "$zipfile" "$url"
  else
    echo "error: need curl or wget to download Emulicious" >&2
    rm -rf "$tmp"
    return 1
  fi

  if command -v unzip >/dev/null 2>&1; then
    unzip -o -q "$zipfile" -d "$dest"
  else
    echo "error: need unzip to extract Emulicious" >&2
    rm -rf "$tmp"
    return 1
  fi
  rm -rf "$tmp"

  if [[ ! -f "$dest/Emulicious.jar" ]]; then
    echo "error: Emulicious.jar not found after extract in $dest" >&2
    return 1
  fi
  echo "$dest/Emulicious.jar"
}

# Find Emulicious or download into tools/emulicious. Prints path.
spirit_ensure_emulicious() {
  local root="$1"
  local found
  if found="$(spirit_find_emulicious "$root")"; then
    echo "$found"
    return 0
  fi
  spirit_download_emulicious "$root"
}

spirit_apply_env() {
  local root="$1"
  export SPIRIT_ROOT="$root"
  export ZGB_PATH="$root/deps/CrossZGB/common"
  export GBDK_HOME="$root/deps/gbdk"
  export PATH="$GBDK_HOME/bin:$PATH"

  local emu
  if emu="$(spirit_find_emulicious "$root" 2>/dev/null)"; then
    export EMULICIOUS_PATH="$emu"
  fi
}
