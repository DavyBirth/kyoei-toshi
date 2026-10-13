param([switch]$CheckOnly)
$ErrorActionPreference='Stop'
if (-not $CheckOnly -and @(Get-Process kyoei,kyoei-orb-trace -ErrorAction SilentlyContinue).Count) {
    throw 'Já existe uma partida aberta. Feche pelo jogo antes deste teste.'
}
Write-Host 'Diagnóstico do orbe: F9 antes de pegar; F9 novamente com o tutorial na tela. Não precisa enviar logs.'
& "$PSScriptRoot\run_native.ps1" -Language en -Width 1280 -Height 720 -InGameDialogs -FitHud -Diagnostics -OrbTrace -CheckOnly:$CheckOnly
exit $LASTEXITCODE
