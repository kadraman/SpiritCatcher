$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not $RepoRoot) { $RepoRoot = $PSScriptRoot }
. (Join-Path $RepoRoot "scripts\lib\env.ps1")

$Root = Get-SpiritRepoRoot -StartPath $RepoRoot
Set-SpiritEnvironment -Root $Root
Assert-SpiritDeps -Root $Root

if (-not $env:BUILD_TYPE) {
    $env:BUILD_TYPE = "Debug"
}
# $env:BUILD_TYPE = "Release"

Set-Location (Join-Path $Root "src")
try {
    make "BUILD_TYPE=$($env:BUILD_TYPE)" gbc
}
finally {
    Set-Location $Root
}
