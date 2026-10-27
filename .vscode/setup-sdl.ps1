$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$dependencyRoot = Join-Path $repositoryRoot "build\deps"
$sdlDirectory = Join-Path $dependencyRoot "SDL2-2.32.10"
$imageDirectory = Join-Path $dependencyRoot "SDL2_image-2.8.12"
$requiredFiles = @(
    (Join-Path $sdlDirectory "include\SDL.h"),
    (Join-Path $sdlDirectory "lib\x64\SDL2.dll"),
    (Join-Path $imageDirectory "include\SDL_image.h"),
    (Join-Path $imageDirectory "lib\x64\SDL2_image.dll")
)

if (@($requiredFiles | Where-Object { -not (Test-Path $_) }).Count -eq 0) {
    Write-Output "SDL2 dependencies are already prepared."
    exit 0
}

$archives = @(
    @{
        Name = "SDL2-devel-2.32.10-VC.zip"
        Url = "https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-devel-2.32.10-VC.zip"
        Sha256 = "af347939395a58b365846aaea27391e69f9ec9d4dd650d6ac40802159b418a6e"
    },
    @{
        Name = "SDL2_image-devel-2.8.12-VC.zip"
        Url = "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.12/SDL2_image-devel-2.8.12-VC.zip"
        Sha256 = "9e14b4166d6c905db18dbe5bbe2288bca17874a0944c70b457cd31b31f0ecf8f"
    }
)

New-Item -ItemType Directory -Path $dependencyRoot -Force | Out-Null
foreach ($archive in $archives) {
    $archivePath = Join-Path $env:TEMP $archive.Name
    Invoke-WebRequest -Uri $archive.Url -OutFile $archivePath
    $actualHash = (Get-FileHash $archivePath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actualHash -ne $archive.Sha256) {
        throw "SHA-256 verification failed for $($archive.Name)."
    }
    Expand-Archive -LiteralPath $archivePath -DestinationPath $dependencyRoot -Force
}

$missingFiles = @($requiredFiles | Where-Object { -not (Test-Path $_) })
if ($missingFiles.Count -gt 0) {
    throw "SDL dependency archive is missing required files: $($missingFiles -join ', ')"
}

Write-Output "Verified SDL2 2.32.10 and SDL2_image 2.8.12 under $dependencyRoot"
