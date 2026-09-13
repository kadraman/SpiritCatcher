# Bootstrap Spirit Catcher deps: CrossZGB submodule, GBDK, host tools, optional Emulicious.
[CmdletBinding()]
param(
    [switch]$Force,
    [switch]$WithEmulicious,
    [switch]$SkipTools
)

$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
. (Join-Path $ScriptDir "lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath (Join-Path $ScriptDir "..")
Set-Location $Root

$versions = Get-SpiritVersions -Root $Root
$CROSSZGB_TAG = $versions["CROSSZGB_TAG"]
$GBDK_RELEASE = $versions["GBDK_RELEASE"]
$EMULICIOUS_URL = $versions["EMULICIOUS_URL"]

Write-Host "==> Repo root: $Root"
Write-Host "==> CrossZGB tag: $CROSSZGB_TAG"
Write-Host "==> GBDK release: $GBDK_RELEASE"

$makefile = Join-Path $Root "deps\CrossZGB\common\src\MakefileCommon"
if (-not (Test-Path $makefile)) {
    Write-Host "==> Initializing CrossZGB submodule..."
    git submodule update --init --recursive deps/CrossZGB
}

Write-Host "==> Checking out CrossZGB $CROSSZGB_TAG..."
git -C (Join-Path $Root "deps\CrossZGB") fetch --tags --quiet
git -C (Join-Path $Root "deps\CrossZGB") checkout -q $CROSSZGB_TAG

$GBDK_ASSET = $versions["GBDK_ASSET_WINDOWS_X64"]
$GBDK_URL = "https://github.com/gbdk-2020/gbdk-2020/releases/download/$GBDK_RELEASE/$GBDK_ASSET"
$GBDK_DIR = Join-Path $Root "deps\gbdk"
$LCC = Join-Path $GBDK_DIR "bin\lcc.exe"
$LCC_UNIX = Join-Path $GBDK_DIR "bin\lcc"

$needGbdk = $true
if (-not $Force -and ((Test-Path $LCC) -or (Test-Path $LCC_UNIX))) {
    Write-Host "==> GBDK already present at deps/gbdk (use -Force to re-download)"
    $needGbdk = $false
}

if ($needGbdk) {
    Write-Host "==> Downloading GBDK $GBDK_RELEASE ($GBDK_ASSET)..."
    $tmp = Join-Path ([System.IO.Path]::GetTempPath()) ("spirit-gbdk-" + [guid]::NewGuid().ToString())
    New-Item -ItemType Directory -Force -Path $tmp | Out-Null
    $archive = Join-Path $tmp $GBDK_ASSET
    Invoke-WebRequest -Uri $GBDK_URL -OutFile $archive

    if (Test-Path $GBDK_DIR) {
        Remove-Item -Recurse -Force $GBDK_DIR
    }
    $depsDir = Join-Path $Root "deps"
    Expand-Archive -Path $archive -DestinationPath $depsDir -Force
    Remove-Item -Recurse -Force $tmp

    if (-not (Test-Path $GBDK_DIR)) {
        throw "Expected $GBDK_DIR after extract"
    }
    Write-Host "==> GBDK installed to deps/gbdk"
}

if (-not $SkipTools) {
    if (-not (Get-Command make -ErrorAction SilentlyContinue)) {
        throw "make is required to build CrossZGB tools (install via MSYS2, Chocolatey, or Git Bash toolchain)"
    }
    Write-Host "==> Building CrossZGB host tools..."
    $toolsDir = Join-Path $Root "deps\CrossZGB\tools"
    $makeArgs = @()
    if (-not (Test-SpiritHasZlib)) {
        Write-Host "==> zlib.h not found; skipping vgm2psg (Spirit Catcher does not use VGM/PSG)"
        $dirs = @(
            Get-ChildItem -Directory $toolsDir |
                Where-Object { $_.Name -ne "vgm2psg" } |
                ForEach-Object { "$($_.Name)/." }
        )
        $makeArgs += "SUBDIRS=$($dirs -join ' ')"
    }
    Push-Location $toolsDir
    try {
        & make @makeArgs
        if ($LASTEXITCODE -ne 0) {
            throw "CrossZGB tools build failed with exit code $LASTEXITCODE"
        }
    }
    finally {
        Pop-Location
    }
}

if ($WithEmulicious) {
    $jar = Ensure-LocalEmulicious -Root $Root -Url $EMULICIOUS_URL
    Write-Host "==> Emulicious.jar ready: $jar"
}

Write-Host "==> Bootstrap complete."
Write-Host "    ZGB_PATH  -> $(Join-Path $Root 'deps\CrossZGB\common')"
Write-Host "    GBDK_HOME -> $(Join-Path $Root 'deps\gbdk')"
Write-Host "    Next: .\build.ps1"
