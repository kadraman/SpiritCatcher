# Shared path / dependency helpers for Spirit Catcher (PowerShell).
# Dot-sourced by scripts/{bootstrap,build,clean,run,setenv}.ps1

function Get-SpiritRepoRoot {
    param([string]$StartPath = $PSScriptRoot)

    $dir = Resolve-Path $StartPath
    while ($null -ne $dir) {
        $versions = Join-Path $dir "deps\VERSIONS"
        if (Test-Path $versions) {
            return $dir.Path
        }
        $parent = Split-Path $dir -Parent
        if ([string]::IsNullOrEmpty($parent) -or $parent -eq $dir) {
            break
        }
        $dir = $parent
    }

    # scripts\lib -> repo root
    $candidate = Resolve-Path (Join-Path $PSScriptRoot "..\..") -ErrorAction SilentlyContinue
    if ($null -ne $candidate -and (Test-Path (Join-Path $candidate "deps\VERSIONS"))) {
        return $candidate.Path
    }

    throw "Could not locate Spirit Catcher repo root (deps/VERSIONS)"
}

function Get-SpiritVersions {
    param([Parameter(Mandatory = $true)][string]$Root)

    $map = @{}
    Get-Content (Join-Path $Root "deps\VERSIONS") | ForEach-Object {
        if ($_ -match '^\s*([A-Z0-9_]+)=(.*)$') {
            $map[$Matches[1]] = $Matches[2].Trim()
        }
    }
    return $map
}

function Assert-SpiritDeps {
    param([Parameter(Mandatory = $true)][string]$Root)

    $makefile = Join-Path $Root "deps\CrossZGB\common\src\MakefileCommon"
    if (-not (Test-Path $makefile)) {
        throw "CrossZGB submodule missing at deps/CrossZGB. Run: git submodule update --init --recursive ; then scripts/bootstrap.ps1"
    }

    $lcc = Join-Path $Root "deps\gbdk\bin\lcc.exe"
    $lccUnix = Join-Path $Root "deps\gbdk\bin\lcc"
    if (-not (Test-Path $lcc) -and -not (Test-Path $lccUnix)) {
        throw "GBDK not found at deps/gbdk. Run: scripts/bootstrap.ps1"
    }
}

function Find-Emulicious {
    param([string]$Root)

    if ($env:EMULICIOUS_PATH) {
        $candidate = $env:EMULICIOUS_PATH
        if (Test-Path $candidate -PathType Container) {
            $jar = Join-Path $candidate "Emulicious.jar"
            $exe = Join-Path $candidate "Emulicious.exe"
            if (Test-Path $jar) { return $jar }
            if (Test-Path $exe) { return $exe }
        }
        elseif (Test-Path $candidate -PathType Leaf) {
            return $candidate
        }
    }

    foreach ($name in @("emulicious", "Emulicious", "Emulicious.jar", "Emulicious.exe")) {
        $cmd = Get-Command $name -ErrorAction SilentlyContinue
        if ($null -ne $cmd) {
            return $cmd.Source
        }
    }

    $homeDir = $env:USERPROFILE
    if (-not $homeDir) { $homeDir = $env:HOME }

    $candidates = @(
        (Join-Path $env:LOCALAPPDATA "Emulicious\Emulicious.exe"),
        (Join-Path $env:LOCALAPPDATA "Emulicious\Emulicious.jar"),
        (Join-Path $env:ProgramFiles "Emulicious\Emulicious.exe"),
        (Join-Path $env:ProgramFiles "Emulicious\Emulicious.jar"),
        (Join-Path ${env:ProgramFiles(x86)} "Emulicious\Emulicious.exe"),
        (Join-Path $homeDir "Emulicious\Emulicious.exe"),
        (Join-Path $homeDir "Emulicious\Emulicious.jar")
    )

    if ($Root) {
        $candidates += @(
            (Join-Path $Root "tools\emulicious\Emulicious.exe"),
            (Join-Path $Root "tools\emulicious\Emulicious.jar")
        )
    }

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path $candidate)) {
            return $candidate
        }
    }

    return $null
}

function Install-Emulicious {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [string]$Url
    )

    if (-not $Url) {
        $versions = Get-SpiritVersions -Root $Root
        $Url = $versions["EMULICIOUS_URL"]
    }

    $dest = Join-Path $Root "tools\emulicious"
    New-Item -ItemType Directory -Force -Path $dest | Out-Null

    $tmp = Join-Path ([System.IO.Path]::GetTempPath()) ("spirit-emulicious-" + [guid]::NewGuid().ToString())
    New-Item -ItemType Directory -Force -Path $tmp | Out-Null
    $zipfile = Join-Path $tmp "emulicious.zip"

    Write-Host "Downloading Emulicious to $dest ..."
    Invoke-WebRequest -Uri $Url -OutFile $zipfile
    Expand-Archive -Path $zipfile -DestinationPath $dest -Force
    Remove-Item -Recurse -Force $tmp

    $jar = Join-Path $dest "Emulicious.jar"
    $exe = Join-Path $dest "Emulicious.exe"
    if (Test-Path $exe) { return $exe }
    if (Test-Path $jar) { return $jar }
    throw "Emulicious not found after extract in $dest"
}

function Ensure-Emulicious {
    param([Parameter(Mandatory = $true)][string]$Root)

    $found = Find-Emulicious -Root $Root
    if ($found) { return $found }
    return Install-Emulicious -Root $Root
}

function Set-SpiritEnvironment {
    param([Parameter(Mandatory = $true)][string]$Root)

    $env:SPIRIT_ROOT = $Root
    $env:ZGB_PATH = Join-Path $Root "deps\CrossZGB\common"
    $env:GBDK_HOME = Join-Path $Root "deps\gbdk"
    $env:PATH = "$(Join-Path $env:GBDK_HOME 'bin');$env:PATH"

    $emu = Find-Emulicious -Root $Root
    if ($emu) {
        $env:EMULICIOUS_PATH = $emu
    }
}
