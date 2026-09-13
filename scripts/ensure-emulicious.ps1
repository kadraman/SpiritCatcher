# Make sure tools/emulicious/Emulicious.jar exists for F5 / Emulicious Debugger.
$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
. (Join-Path $ScriptDir "lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath (Join-Path $ScriptDir "..")
$versions = Get-SpiritVersions -Root $Root
$jar = Ensure-LocalEmulicious -Root $Root -Url $versions["EMULICIOUS_URL"]
Write-Host "Emulicious.jar: $jar"
