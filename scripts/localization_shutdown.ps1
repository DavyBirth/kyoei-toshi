param([switch]$CheckpointOnly)
$ErrorActionPreference = 'Continue'
$projectRoot = Split-Path $PSScriptRoot
$translationWorkspace = 'G:\kyoei-toshi-work\translation\retorno-gpt\codex-localization'
$checkpointPython = 'C:\Users\davyd\AppData\Local\Python\pythoncore-3.14-64\python.exe'
$shutdownLog = Join-Path $projectRoot 'build\localization-shutdown.log'
$checkpointArguments = @((Join-Path $projectRoot 'tools\checkpoint_localization.py'), $translationWorkspace)
if (-not $CheckpointOnly) { $checkpointArguments += '--publish' }
& $checkpointPython @checkpointArguments *>> $shutdownLog
$checkpointResult = $LASTEXITCODE
"$(Get-Date -Format o) checkpoint exit=$checkpointResult checkpointOnly=$CheckpointOnly" | Add-Content -LiteralPath $shutdownLog
if (-not $CheckpointOnly) {
    # Davy explicitly authorized total shutdown at 03:00. Do not use /f.
    # Existing audited checkpoints remain available if final publication fails.
    & "$env:SystemRoot\System32\shutdown.exe" /s /t 0
}
