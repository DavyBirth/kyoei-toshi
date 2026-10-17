param([switch]$Watch,[ValidateRange(30,3600)][int]$IntervalSeconds=600,
      [string]$SessionPath='',[ValidateRange(1,99)][double]$ReservePercent=15)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path $PSScriptRoot
$usageRoot=Join-Path $projectRoot 'build\overnight'
New-Item -ItemType Directory -Force -Path $usageRoot | Out-Null
$anchor=Join-Path $usageRoot 'current-session-path.txt'
if(-not $SessionPath -and $env:CODEX_THREAD_ID) {
    $session=Get-ChildItem -LiteralPath (Join-Path $env:USERPROFILE '.codex\sessions') -Recurse -File -Filter "*$($env:CODEX_THREAD_ID)*.jsonl" | Select-Object -First 1
    if($session){$SessionPath=$session.FullName}
} elseif(-not $SessionPath -and (Test-Path -LiteralPath $anchor)) {
    $SessionPath=(Get-Content -LiteralPath $anchor -Raw).Trim()
}
if(-not $SessionPath){throw 'Informe a sessao correta; nao selecionar automaticamente auxiliares mais novos.'}
$SessionPath=(Get-Item -LiteralPath $SessionPath).FullName
$SessionPath | Set-Content -LiteralPath $anchor -Encoding UTF8
do {
    $payload=& python (Join-Path $projectRoot 'tools\read_codex_usage.py') $SessionPath --reserve $ReservePercent
    if($LASTEXITCODE -ne 0){throw 'Nao foi possivel ler telemetria recente da sessao.'}
    $sample=$payload | ConvertFrom-Json
    if(-not $sample.checked_at){throw 'Resposta de telemetria invalida.'}
    $payload | Set-Content -LiteralPath (Join-Path $usageRoot 'usage-current.json') -Encoding UTF8
    Add-Content -LiteralPath (Join-Path $usageRoot 'usage-history.jsonl') -Encoding UTF8 -Value $payload
    if(-not $Watch){Write-Output $payload;break}
    for($elapsed=0;$elapsed -lt $IntervalSeconds;$elapsed+=5) {
        if(Test-Path -LiteralPath (Join-Path $usageRoot 'stop-monitor')){return}
        Start-Sleep -Seconds ([Math]::Min(5,$IntervalSeconds-$elapsed))
    }
} while(-not (Test-Path -LiteralPath (Join-Path $usageRoot 'stop-monitor')))
