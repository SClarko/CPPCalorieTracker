param(
    [ValidateSet("Debug","Release")]
    [string]$Config = "Debug"
)

$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $root "build"

if (!(Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Push-Location $buildDir

cmake ..
cmake --build . --config $Config

Pop-Location

Write-Host "Build finished ($Config)."
Write-Host "Run: .\build\$Config\CPPCalorieTracker.exe"