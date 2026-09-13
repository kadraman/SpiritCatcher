# Export project-local CrossZGB / GBDK / Emulicious environment.
# Usage: . .\scripts\setenv.ps1

$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
. (Join-Path $ScriptDir "lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath (Join-Path $ScriptDir "..")
Set-SpiritEnvironment -Root $Root

if (-not (Test-Path (Join-Path $env:ZGB_PATH "src\MakefileCommon"))) {
    Write-Warning "CrossZGB submodule missing; run: git submodule update --init --recursive"
}
$lcc = Join-Path $env:GBDK_HOME "bin\lcc.exe"
$lccUnix = Join-Path $env:GBDK_HOME "bin\lcc"
if (-not (Test-Path $lcc) -and -not (Test-Path $lccUnix)) {
    Write-Warning "GBDK missing; run: .\scripts\bootstrap.ps1"
}

Write-Host "ZGB_PATH=$env:ZGB_PATH"
Write-Host "GBDK_HOME=$env:GBDK_HOME"
if ($env:EMULICIOUS_PATH) {
    Write-Host "EMULICIOUS_PATH=$env:EMULICIOUS_PATH"
}
