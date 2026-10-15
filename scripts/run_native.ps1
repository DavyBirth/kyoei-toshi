# Run only the prepared Kyoei, with separate writable data. No shadPS4 executable.
param([string]$Work = '', [switch]$CheckOnly, [ValidateSet('ja','en','pt-BR')][string]$Language='ja',
      [ValidateRange(640,8192)][int]$Width=1280,[ValidateRange(360,4320)][int]$Height=720,[switch]$Fullscreen,[switch]$InGameDialogs,[switch]$Diagnostics,[switch]$PerformanceStats,[switch]$FitHud,[switch]$BatchReadback,[switch]$OrbTrace,[switch]$OrbTraceNext,[switch]$OriginalHud)
$ErrorActionPreference = 'Stop'
$kyoeiRoot = Split-Path $PSScriptRoot
$kyoeiExecutable=Join-Path "$kyoeiRoot\build" $(if ($OrbTraceNext) { 'kyoei-orb-trace-next.exe' } elseif ($OrbTrace) { 'kyoei-orb-trace.exe' } else { 'kyoei.exe' })
if ($OrbTraceNext) { $kyoeiExecutable=Join-Path "$kyoeiRoot\build\orb-pending" 'kyoei-orb-trace-next.exe' }
$kyoeiProof = Get-Content -LiteralPath "$kyoeiRoot\docs\native-prepare.json" -Raw | ConvertFrom-Json
if (-not $Work) { $Work = Split-Path $kyoeiProof.prepared_out }
if ($kyoeiProof.title -ne 'CUSA03562' -or $kyoeiProof.version -ne '01.03') { throw 'Identidade inválida' }
if ([IO.Path]::GetFullPath("$Work\out") -ne [IO.Path]::GetFullPath($kyoeiProof.prepared_out)) { throw 'Pasta preparada diferente da prova' }
$kyoeiFonts = "$kyoeiRoot\third_party\godzilla-core\third_party\shadPS4\src\imgui\renderer\fonts"
$env:KYOEI_GUI_FONT_DIR=$kyoeiFonts
foreach ($kyoeiRequired in @($kyoeiExecutable, "$Work\out\boot-linked.bin", "$Work\out\entry.bin", "$Work\out\imports.tsv", "$Work\out\link.json", "$kyoeiFonts\NotoSans-Regular.ttf", "$kyoeiFonts\NotoSansCJK-Regular.ttc", "$($kyoeiProof.source)\eboot.bin", "$($kyoeiProof.source)\sce_sys\param.sfo")) {
    if (-not (Test-Path -LiteralPath $kyoeiRequired -PathType Leaf)) { throw "Arquivo necessario ausente: $kyoeiRequired" }
}
if (-not $CheckOnly) {
    $kyoeiExisting = @(Get-CimInstance Win32_Process | Where-Object {
        $_.Name -in @('kyoei.exe','kyoei-orb-trace.exe','kyoei-orb-trace-next.exe')
    })
    if ($kyoeiExisting.Count) {
        throw 'J? existe uma partida do Kyoei aberta. Feche pelo pr?prio jogo antes de iniciar outra.'
    }
}
$env:KYOEI_LANGUAGE=$Language
$env:KYOEI_PIPELINE_CACHE_DIR=Join-Path $Work 'cache\vulkan'
$env:KYOEI_RASTER_TRACE=if ($OrbTrace -or $OrbTraceNext) { '1' } else { '0' }
if ($InGameDialogs -and $Language -ne 'en') { throw 'A prévia dos diálogos dentro do jogo está disponível em inglês.' }
$env:KYOEI_IN_GAME_DIALOGS=if ($InGameDialogs) { '1' } else { '0' }
if ($Diagnostics) { $env:KYOEI_HUD_TRACE='1'; $env:KYOEI_FRAME_STATS='1' }
if ($PerformanceStats) { $env:KYOEI_FRAME_STATS='1'; $env:KYOEI_HUD_TRACE='0' }
if ($Language -eq 'en' -and -not $OriginalHud) { $FitHud=$true }
$env:KYOEI_HUD_FIT=if ($FitHud) { '1' } else { '0' }
$env:KYOEI_BATCH_READBACK=if ($BatchReadback) { '1' } else { '0' }
if ($FitHud) {
    if ($Language -ne 'en') { throw 'A prévia do ajuste da HUD está disponível em inglês.' }
    $kyoeiMetrics="$kyoeiRoot\build\hud-native-metrics-01.bin"
    if (-not (Test-Path -LiteralPath $kyoeiMetrics -PathType Leaf)) { throw 'Métricas nativas da HUD ausentes.' }
    $env:KYOEI_HUD_METRICS=$kyoeiMetrics
}
Remove-Item Env:KYOEI_TRANSLATION_ARCHIVE -ErrorAction SilentlyContinue
if ($Language -ne 'ja') {
    $kyoeiTranslation=Get-Content -LiteralPath "$kyoeiRoot\docs\m4\runtime-preview.json" -Raw | ConvertFrom-Json
    if ($kyoeiTranslation.title -ne 'CUSA03562' -or $kyoeiTranslation.version -ne '01.03') { throw 'Traducao para outro jogo ou versao' }
    $kyoeiArchive=$kyoeiTranslation.languages.$Language
    if (-not $kyoeiArchive -or -not (Test-Path -LiteralPath $kyoeiArchive.archive -PathType Leaf)) { throw 'Previa de idioma ainda nao preparada' }
    if ((Get-Item -LiteralPath $kyoeiArchive.archive).Length -ne $kyoeiArchive.bytes) { throw 'Pacote de traducao alterado; reconstruir a previa' }
    $env:KYOEI_TRANSLATION_ARCHIVE=$kyoeiArchive.archive
}
$env:KYOEI_WINDOW_WIDTH=[string]$Width
$env:KYOEI_WINDOW_HEIGHT=[string]$Height
$env:KYOEI_FULLSCREEN=if ($Fullscreen) { '1' } else { '0' }
$env:KYOEI_WINDOW_ICON="$kyoeiRoot\build\launcher-assets\game.ico"
if ($CheckOnly) {
    Write-Output "Launcher OK; CUSA03562 01.03; idioma: $Language; saida: ${Width}x${Height}; fullscreen: $Fullscreen; dialogos no jogo: $InGameDialogs; diagnostico: $Diagnostics; ajuste HUD: $FitHud; readback agrupado: $BatchReadback; saves: $Work\user; nenhum jogo iniciado."
    exit 0
}
if (-not $env:GZ_PRESENT_MODE) { $env:GZ_PRESENT_MODE = 'Fifo' }
if (-not $env:GZ_READBACK_LINEAR) { $env:GZ_READBACK_LINEAR = '1' }
if (-not $env:GZ_SYSTEM_FONT) { $env:GZ_SYSTEM_FONT = "$kyoeiFonts\NotoSans-Regular.ttf" }
if (-not $env:GZ_SYSTEM_FONT_JP) { $env:GZ_SYSTEM_FONT_JP = "$kyoeiFonts\NotoSansCJK-Regular.ttc" }
New-Item -ItemType Directory -Force -Path "$Work\logs" | Out-Null
$kyoeiLog = Join-Path "$Work\logs" ((Get-Date -Format 'yyyyMMdd_HHmmss')+'.log')
& $kyoeiExecutable "$Work\out" --app0 $kyoeiProof.source --user "$Work\user" --log $kyoeiLog
Write-Host "Código de saída: $LASTEXITCODE; registro: $kyoeiLog"
exit $LASTEXITCODE
