# User-run diagnostic launch; never invoked for gameplay by the agent.
param([switch]$CheckOnly,[switch]$NativeDialogs)
$ErrorActionPreference='Stop'
if (-not $CheckOnly -and @(Get-Process kyoei -ErrorAction SilentlyContinue).Count) {
    throw 'Já existe uma partida aberta. Feche pelo próprio jogo antes de iniciar este teste.'
}
$kyoeiArguments=@{Language='en';Width=1280;Height=720;Diagnostics=$true;CheckOnly=$CheckOnly}
if (-not $NativeDialogs) { $kyoeiArguments.InGameDialogs=$true }
& "$PSScriptRoot\run_native.ps1" @kyoeiArguments
exit $LASTEXITCODE
