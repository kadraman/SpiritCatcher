# Spirit Catcher DX

An action platformer for the Nintendo Game Boy Color written using [CrossZGB](https://github.com/gbdk-2020/CrossZGB).

## Prerequisites

- Git (with submodule support)
- `make` and a C/C++ toolchain (Xcode CLT, build-essential, or MSYS2/MinGW on Windows)
- zlib development headers (`zlib.h`) only if you want CrossZGB's `vgm2psg` tool; Spirit Catcher does not use VGM files, and bootstrap skips that tool when zlib is missing
- Python 3 with `pillow` and `wave` (used by some CrossZGB asset tools)
- Java 21+ (only needed to run Emulicious)

### Python (Pillow)

`wave` is part of the Python standard library; you mainly need to install **Pillow**.

Check:

```bash
python3 -c "import PIL, wave; print('ok', PIL.__version__)"
```

If that prints `ok …`, you are set. Otherwise install:

```bash
# macOS / Linux
python3 -m pip install --upgrade pip
python3 -m pip install pillow

# Windows (PowerShell)
py -3 -m pip install --upgrade pip
py -3 -m pip install pillow
```

If `pip` is not available:

```bash
# macOS (Homebrew)
brew install python
python3 -m pip install pillow

# Debian / Ubuntu
sudo apt-get install -y python3 python3-pip python3-pillow
# or: python3 -m pip install pillow
```

### Java 21+

Needed to launch Emulicious (`.jar`). Newer JDKs (e.g. 22+) are fine.

Check:

```bash
java -version
```

You want a line like `openjdk version "21…"` (or newer).

Install:

```bash
# macOS (Homebrew)
brew install openjdk@21
# Then either follow the caveats brew prints, or:
export PATH="$(brew --prefix openjdk@21)/bin:$PATH"

# Debian / Ubuntu
sudo apt-get install -y openjdk-21-jdk

# Windows (winget)
winget install Microsoft.OpenJDK.21
# or: winget install EclipseAdoptium.Temurin.21.JDK
```

Run `java -version` again to confirm.

## Setup

```bash
git clone --recurse-submodules <repo-url>
cd SpiritCatcher

# Linux / macOS
./scripts/bootstrap.sh --with-emulicious

# Windows (PowerShell)
.\scripts\bootstrap.ps1 -WithEmulicious
```

This will:

1. Check out the CrossZGB submodule at the pinned tag (`deps/VERSIONS`)
2. Download GBDK into `deps/gbdk` (gitignored)
3. Build CrossZGB host tools
4. Optionally install Emulicious into `tools/emulicious` if it is not already on your system

No system-wide `ZGB_HOME` install is required. Engine source lives at `deps/CrossZGB` for browsing and debugging.

If you already cloned without submodules:

```bash
git submodule update --init --recursive
./scripts/bootstrap.sh
```

## Building

```bash
# Linux / macOS
./build.sh

# Windows
.\build.ps1
```

(`./scripts/build.sh` / `.\scripts\build.ps1` work the same.)

The ROM is written to `bin/gbc/` (`SpiritDX_Debug.gbc` for Debug, `SpiritDX.gbc` for Release).

```bash
./scripts/clean.sh   # or .\scripts\clean.ps1
```

## Running in Emulicious

```bash
./run.sh     # or .\run.ps1
```

Emulicious is resolved in this order:

1. `EMULICIOUS_PATH` (jar, exe, or directory)
2. `emulicious` / `Emulicious` on `PATH`
3. Common OS install locations
4. `tools/emulicious/` in this repo
5. Automatic download into `tools/emulicious/` if still missing

## VS Code / Cursor (Emulicious Debugger)

Install the recommended extension when prompted, or from the Extensions view:

- **Emulicious Debugger** — `emulicious.emulicious-debugger`
- **C/C++** — `ms-vscode.cpptools` (for breakpoints in `.c` sources)

F5 launches `${workspaceFolder}/tools/emulicious/Emulicious.jar`. A system-wide Emulicious.exe is not enough. Ensure the local jar once (or let F5 copy/download it):

```bash
./scripts/bootstrap.sh --with-emulicious
# or: ./scripts/ensure-emulicious.sh
```

### Launch with F5 (preferred)

1. Open the **Run and Debug** view (`Cmd+Shift+D` / `Ctrl+Shift+D`).
2. Select **Debug** (builds `SpiritDX_Debug.gbc` then launches Emulicious).
3. Press **F5**.

Cursor/VS Code starts Emulicious via `tools/emulicious/Emulicious.jar`, loads the ROM, and connects the debugger. The **Debug Console** shows `EMU_printf` output (lines prefixed with `DBG363183` while debugging the hit bug).

Other launch configs:

| Config | Purpose |
| --- | --- |
| **Debug** | Build Debug ROM + launch Emulicious + attach debugger |
| **Release** | Build Release ROM + launch Emulicious + attach debugger |
| **Attach to Emulicious** | Attach to an already-running Emulicious (enable **Tools → Remote Debugging** in Emulicious first) |

Debug symbols come from the CrossZGB Debug build (`.cdb` / `.sym` next to the ROM in `bin/gbc/`).

### macOS note

If launch fails with “Operation Not Permitted”, grant permissions to `/System/Library/CoreServices/Jar Launcher.app` (see the [Emulicious Debugger README](https://github.com/Calindro/emulicious-debugger)).

### Running without the debugger

```bash
./run.sh     # or .\run.ps1
```

## Environment variables

`source ./scripts/setenv.sh` / `. .\scripts\setenv.ps1` export project-local paths (also applied automatically by build/clean/run):

| Variable | Value |
| --- | --- |
| `ZGB_PATH` | `<repo>/deps/CrossZGB/common` |
| `GBDK_HOME` | `<repo>/deps/gbdk` |
| `EMULICIOUS_PATH` | resolved Emulicious jar/exe when found |

Tooling lives under `scripts/` (`bootstrap`, `build`, `clean`, `run`, `setenv`, `lib`). Root `build` / `run` are thin wrappers.

----

Kevin A. Lee (kadraman)
