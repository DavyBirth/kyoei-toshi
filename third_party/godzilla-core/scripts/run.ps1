# Build the loader, prepare the game files (only what changed) and run the game.
# The log goes to <Work>\logs\<date_time>.log. Extra arguments go to gzport.exe
# (e.g. -- --stubs-return-zero).
param(
    [string]$Work = 'G:\godzilla-work-codex',
    [Parameter(ValueFromRemainingArguments)] [string[]]$GzArgs
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot
. (Join-Path $PSScriptRoot 'runtime_defaults.ps1')
# Tools installed after this terminal opened are found through the saved PATH.
$env:Path = $env:Path + ';' + [Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [Environment]::GetEnvironmentVariable('Path', 'User')

$build = Join-Path $root 'build'
if (-not (Test-Path (Join-Path $build 'build.ninja'))) {
    cmake -S $root -B $build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
    if ($LASTEXITCODE) { throw 'cmake configure failed' }
}
cmake --build $build --parallel 4
if ($LASTEXITCODE) { throw 'build failed' }

& (Join-Path $PSScriptRoot 'prepare_all.ps1') -Work $Work

$logs = Join-Path $Work 'logs'
New-Item -ItemType Directory -Force $logs | Out-Null
$log = Join-Path $logs ((Get-Date -Format 'yyyyMMdd_HHmmss') + '.log')
Write-Host "Log: $log"
& (Join-Path $build 'gzport.exe') (Join-Path $Work 'out') --log $log @GzArgs
Write-Host "gzport exit code: $LASTEXITCODE"
