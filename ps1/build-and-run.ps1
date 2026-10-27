param(
    [switch]$BuildOnly
)

$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$setupScript = Join-Path $repositoryRoot ".vscode\setup-sdl.ps1"
$buildScript = Join-Path $repositoryRoot ".vscode\build.ps1"
$executablePath = Join-Path $repositoryRoot "build\finder.exe"

foreach ($scriptPath in @($setupScript, $buildScript)) {
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $scriptPath
    if ($LASTEXITCODE -ne 0) {
        throw "Script failed with exit code ${LASTEXITCODE}: $scriptPath"
    }
}

if ($BuildOnly) {
    exit 0
}

if (-not (Test-Path -LiteralPath $executablePath)) {
    throw "Build completed without producing $executablePath."
}

$process = Start-Process -FilePath $executablePath `
    -WorkingDirectory $repositoryRoot -PassThru
Write-Output "Started TheFinder (PID $($process.Id))."
