param(
    [ValidateSet("Debug","Release")]
    [string]$Config = "Debug"
)

$ErrorActionPreference = "Stop"

# Build first
.\build.ps1 -Config $Config

# Run from the build directory so ../data/... works
Push-Location ".\build"
& ".\$Config\CPPCalorieTracker.exe"
Pop-Location