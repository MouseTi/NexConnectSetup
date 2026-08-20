# Build script for NexConnect Setup

param(
    [Parameter()]
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = "Stop"

Write-Host "Building NexConnect Setup ($Configuration)..." -ForegroundColor Cyan

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location "build"

# Generate Visual Studio project
Write-Host "Generating Visual Studio project..." -ForegroundColor Yellow
cmake .. -G "Visual Studio 17 2022" -A x64

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake generation failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Build
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build . --config $Configuration

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Set-Location ..

# Copy output
$outputPath = "build\$Configuration\NexConnect-Setup.exe"
if (Test-Path $outputPath) {
    Copy-Item $outputPath "." -Force
    Write-Host ""
    Write-Host "Build successful!" -ForegroundColor Green
    Write-Host "Output: NexConnect-Setup.exe" -ForegroundColor Green
    
    $size = (Get-Item "NexConnect-Setup.exe").Length / 1MB
    Write-Host "Size: $([math]::Round($size, 2)) MB" -ForegroundColor Cyan
} else {
    Write-Host "Build output not found!" -ForegroundColor Red
    exit 1
}
