param([switch]$CheckOnly,[switch]$Next)
$ErrorActionPreference='Stop'
Write-Host 'Diagnóstico do orbe: F9 antes de pegar; F9 novamente com o tutorial na tela. Não precisa enviar logs.'
& "$PSScriptRoot\run_native.ps1" -Language en -Width 1280 -Height 720 -InGameDialogs -FitHud -Diagnostics -OrbTrace -OrbTraceNext:$Next -CheckOnly:$CheckOnly
exit $LASTEXITCODE
