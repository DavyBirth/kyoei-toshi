# Synthetic pixels through the real GPU presenter; never enters the campaign.
param([ValidateSet('Default','Fsr1')][string]$Upscaling='Default',
      [ValidateSet(1920,3840)][int]$Width=3840,
      [ValidateSet(1080,2160)][int]$Height=2160,
      [ValidatePattern('^[A-Za-z0-9_-]+$')][string]$Run=(Get-Date -Format 'yyyyMMdd_HHmmss_fff'))
$ErrorActionPreference='Stop'
$kyoeiGraphicsRoot=Split-Path $PSScriptRoot
if (@(Get-Process kyoei -ErrorAction SilentlyContinue).Count) { throw 'Feche a partida antes do teste GPU.' }
if ($Width*9 -ne $Height*16) { throw 'O teste requer proporcao16:9.' }
$kyoeiGraphicsProof=Get-Content -LiteralPath "$kyoeiGraphicsRoot/docs/native-prepare.json" -Raw | ConvertFrom-Json
$kyoeiGraphicsTest=Join-Path $kyoeiGraphicsRoot "build/research/graphics/$Run"
if (Test-Path -LiteralPath $kyoeiGraphicsTest) { throw 'Pasta do teste ja existe; use outro Run.' }
New-Item -ItemType Directory -Path "$kyoeiGraphicsTest/user" -Force | Out-Null
$env:SDL_AUDIO_DRIVER='dummy'
$env:KYOEI_GRAPHICS_SELF_TEST='1'
$env:KYOEI_CACHE_SELF_TEST='0'
$env:KYOEI_COOPERATIVE_SELF_TEST='0'
$env:KYOEI_WINDOW_WIDTH=[string]$Width
$env:KYOEI_WINDOW_HEIGHT=[string]$Height
$env:KYOEI_FULLSCREEN='0'
$env:KYOEI_FRAME_STATS='0'
$env:GZ_FSR1=if ($Upscaling -eq 'Fsr1') { '1' } else { '0' }
$env:GZ_RCAS='1'
$env:GZ_RCAS_ATTENUATION='250'
$env:GZ_PRESENT_MODE='Fifo'
$env:GZ_VBLANK_HZ='60'
$env:GZ_READBACK_LINEAR='1'
$env:KYOEI_PIPELINE_CACHE_DIR=Join-Path $kyoeiGraphicsRoot 'build/research/graphics/vulkan'
$env:KYOEI_GUI_FONT_DIR=Join-Path $kyoeiGraphicsRoot 'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts'
$kyoeiGraphicsExecutable=Join-Path $kyoeiGraphicsRoot 'build/kyoei.exe'
Push-Location $kyoeiGraphicsTest
try {
    & $kyoeiGraphicsExecutable $kyoeiGraphicsProof.prepared_out --app0 $kyoeiGraphicsProof.source --user "$kyoeiGraphicsTest/user" --log "$kyoeiGraphicsTest/test.log" --gpu-test > "$kyoeiGraphicsTest/console.log" 2>&1
    if ($LASTEXITCODE -ne 0) { throw "Teste GPU falhou ($LASTEXITCODE). Registro: $kyoeiGraphicsTest/test.log" }
    Get-Content -LiteralPath "$kyoeiGraphicsTest/test.log" | Select-String 'GRAPHICS TEST: PASS'
} finally { Pop-Location }
