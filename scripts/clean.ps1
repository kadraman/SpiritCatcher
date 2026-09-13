$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
. (Join-Path $ScriptDir "lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath (Join-Path $ScriptDir "..")
Set-SpiritEnvironment -Root $Root
Assert-SpiritDeps -Root $Root

if (-not $env:BUILD_TYPE) {
    $env:BUILD_TYPE = "Debug"
}
# $env:BUILD_TYPE = "Release"

Set-Location (Join-Path $Root "src")
try {
    make "BUILD_TYPE=$($env:BUILD_TYPE)" gbc-clean
}
finally {
    Set-Location $Root
}

$obj = Join-Path $Root "obj"
if (Test-Path $obj) {
    Remove-Item -Path $obj -Recurse -Force
}
