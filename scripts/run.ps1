# Run the built GBC ROM in Emulicious (discover env/PATH, else download to tools/emulicious).
$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
. (Join-Path $ScriptDir "lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath (Join-Path $ScriptDir "..")
Set-Location $Root

if (-not $env:BUILD_TYPE) {
    $env:BUILD_TYPE = "Debug"
}

if ($env:BUILD_TYPE -match "Debug") {
    $Rom = Join-Path $Root "bin\gbc\rom_Debug.gbc"
}
else {
    $Rom = Join-Path $Root "bin\gbc\rom.gbc"
}

if (-not (Test-Path $Rom)) {
    throw "ROM not found: $Rom. Build first with: .\build.ps1 (or .\scripts\build.ps1)"
}

$Emu = Ensure-Emulicious -Root $Root
$env:EMULICIOUS_PATH = $Emu

Write-Host "Launching $Rom with $Emu"

if ($Emu -like "*.jar") {
    if (-not (Get-Command java -ErrorAction SilentlyContinue)) {
        throw "Java is required to run Emulicious.jar"
    }
    & java -jar $Emu $Rom
}
else {
    & $Emu $Rom
}
