# Isolated native session for the owner's reported UI defects.
$ErrorActionPreference='Stop'
$kyoeiRoot=Split-Path $PSScriptRoot
if (Get-Process kyoei -ErrorAction SilentlyContinue) { throw 'Close the existing game first' }
$kyoeiUser=Join-Path $kyoeiRoot 'build/ui-review-user'
if (-not (Test-Path -LiteralPath $kyoeiUser)) {
    New-Item -ItemType Directory -Path $kyoeiUser | Out-Null
    Copy-Item -LiteralPath 'G:/kyoei-toshi-work/native-05/user/savedata' -Destination $kyoeiUser -Recurse
}
$env:KYOEI_LANGUAGE='en'
$env:KYOEI_TRANSLATION_ARCHIVE=Join-Path $kyoeiRoot 'build/City-Shrouded-in-Shadow-Portable/languages/en/common.psarc'
$env:KYOEI_ARCHIVE_DELTA=Join-Path $kyoeiRoot 'build/ui-corrections.kydelta'
$env:KYOEI_PROMPT_PACK=Join-Path $kyoeiRoot 'build/prompts-en.kypack'
$env:KYOEI_PROMPT_MODE='auto'
$env:KYOEI_INPUT_TEST='1'
$env:KYOEI_WRAP_TRACE='1'
$env:KYOEI_PIPELINE_CACHE_DIR=Join-Path $kyoeiRoot 'build/ui-review-cache'
$env:KYOEI_HUD_FIT='1'
$env:KYOEI_HUD_METRICS=Join-Path $kyoeiRoot 'build/hud-native-metrics-01.bin'
$env:KYOEI_NAME_STORE=Join-Path $kyoeiUser 'character-names.bin'
$env:KYOEI_TITLE_EXIT='1'
$env:KYOEI_TITLE_EXIT_ART=Join-Path $kyoeiRoot 'build/City-Shrouded-in-Shadow-Portable/assets/title-exit.kyart'
$env:KYOEI_IN_GAME_DIALOGS='1'
$env:KYOEI_GUI_FONT_DIR=Join-Path $kyoeiRoot 'build/City-Shrouded-in-Shadow-Portable/assets/fonts'
$env:KYOEI_MOUSE_CAMERA='0'
$env:KYOEI_WINDOW_WIDTH='1280'
$env:KYOEI_WINDOW_HEIGHT='720'
$env:KYOEI_FULLSCREEN='0'
$env:GZ_READBACK_LINEAR='1'
$env:GZ_PRESENT_MODE='Fifo'
$kyoeiApp=(Get-Content -LiteralPath (Join-Path $kyoeiRoot 'docs/native-prepare.json') -Raw | ConvertFrom-Json).source
$kyoeiArguments=@('G:/kyoei-toshi-work/native-05/out','--app0',('"'+$kyoeiApp+'"'),'--user',('"'+$kyoeiUser+'"'),'--log',('"'+(Join-Path $kyoeiRoot 'build/ui-review.log')+'"'))
$kyoeiProcess=Start-Process -FilePath (Join-Path $kyoeiRoot 'build/kyoei.exe') -ArgumentList $kyoeiArguments -WorkingDirectory $kyoeiRoot -WindowStyle Hidden -PassThru
$kyoeiProcess.Id | Set-Content -LiteralPath (Join-Path $kyoeiRoot 'build/ui-review.pid')
Write-Output "Isolated UI review process: $($kyoeiProcess.Id)"
