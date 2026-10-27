$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$compiler = Get-Command "g++" -ErrorAction SilentlyContinue
$gendef = Get-Command "gendef" -ErrorAction SilentlyContinue
$dlltool = Get-Command "dlltool" -ErrorAction SilentlyContinue
$dependencyRoot = Join-Path $repositoryRoot "build\deps"
$sdlDirectory = Join-Path $dependencyRoot "SDL2-2.32.10"
$imageDirectory = Join-Path $dependencyRoot "SDL2_image-2.8.12"
$sdlIncludePath = Join-Path $sdlDirectory "include"
$imageIncludePath = Join-Path $imageDirectory "include"
$sdlLibraryPath = Join-Path $sdlDirectory "lib\x64"
$imageLibraryPath = Join-Path $imageDirectory "lib\x64"
$outputDirectory = Join-Path $repositoryRoot "build"
$outputPath = Join-Path $outputDirectory "finder.exe"
$importLibraryDirectory = Join-Path $outputDirectory "mingw-imports"

if ($null -eq $compiler) {
    throw "MinGW g++ was not found on PATH."
}
if ($null -eq $gendef -or $null -eq $dlltool) {
    throw "MinGW gendef and dlltool are required to prepare SDL import libraries."
}
$requiredFiles = @(
    (Join-Path $sdlIncludePath "SDL.h"),
    (Join-Path $imageIncludePath "SDL_image.h"),
    (Join-Path $sdlLibraryPath "SDL2.dll"),
    (Join-Path $imageLibraryPath "SDL2_image.dll")
)
$missingFiles = @($requiredFiles | Where-Object { -not (Test-Path $_) })
if ($missingFiles.Count -gt 0) {
    throw "SDL2 dependencies are missing. Run the Install SDL2 dependencies task."
}

New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
New-Item -ItemType Directory -Path $importLibraryDirectory -Force | Out-Null
$sdlDll = Join-Path $sdlLibraryPath "SDL2.dll"
$imageDll = Join-Path $imageLibraryPath "SDL2_image.dll"
Push-Location $importLibraryDirectory
try {
    foreach ($dllPath in @($sdlDll, $imageDll)) {
        & $gendef.Source $dllPath
        if ($LASTEXITCODE -ne 0) {
            throw "Could not read exports from $dllPath."
        }
    }

    & $dlltool.Source -d "SDL2.def" -D "SDL2.dll" -l "libSDL2.a"
    if ($LASTEXITCODE -ne 0) {
        throw "Could not create the SDL2 MinGW import library."
    }
    & $dlltool.Source -d "SDL2_image.def" -D "SDL2_image.dll" -l "libSDL2_image.a"
    if ($LASTEXITCODE -ne 0) {
        throw "Could not create the SDL2_image MinGW import library."
    }
}
finally {
    Pop-Location
}

$compilerArguments = @(
    "-std=c++17",
    "-g",
    "-DSDL_MAIN_HANDLED",
    "-static",
    "-static-libgcc",
    "-static-libstdc++",
    "-I$sdlIncludePath",
    "-I$imageIncludePath",
    "-I$repositoryRoot\inc",
    "src/Main.cpp",
    "src/Game.cpp",
    "src/Map.cpp",
    "src/Player.cpp",
    "src/Enemy.cpp",
    "src/Assets.cpp",
    "src/Menu.cpp",
    "-L$importLibraryDirectory",
    "-L$sdlLibraryPath",
    "-L$imageLibraryPath",
    "-lSDL2_image",
    "-lSDL2",
    "-lwinmm",
    "-o",
    $outputPath
)

Push-Location $repositoryRoot
try {
    & $compiler.Source @compilerArguments
    if ($LASTEXITCODE -ne 0) {
        throw "g++ failed with exit code $LASTEXITCODE."
    }
}
finally {
    Pop-Location
}

foreach ($libraryPath in @($sdlLibraryPath, $imageLibraryPath)) {
    Get-ChildItem -Path $libraryPath -Filter "*.dll" -Recurse |
        Copy-Item -Destination $outputDirectory -Force
}

Write-Output "Built $outputPath"
