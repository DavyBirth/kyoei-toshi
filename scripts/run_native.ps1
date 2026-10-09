# Run only the prepared Kyoei, with separate writable data. No shadPS4 executable.
param([string]$Work = '')
$ErrorActionPreference = 'Stop'
$kyoeiRoot = Split-Path $PSScriptRoot
$kyoeiProof = Get-Content -LiteralPath "$kyoeiRoot\docs\native-prepare.json" -Raw | ConvertFrom-Json
if (-not $Work) { $Work = Split-Path $kyoeiProof.prepared_out }
if ($kyoeiProof.title -ne 'CUSA03562' -or $kyoeiProof.version -ne '01.03') { throw 'Identidade inválida' }
if ([IO.Path]::GetFullPath("$Work\out") -ne [IO.Path]::GetFullPath($kyoeiProof.prepared_out)) { throw 'Pasta preparada diferente da prova' }
$kyoeiFonts = "$kyoeiRoot\third_party\godzilla-core\third_party\shadPS4\src\imgui\renderer\fonts"
if (-not $env:GZ_PRESENT_MODE) { $env:GZ_PRESENT_MODE = 'Fifo' }
if (-not $env:GZ_SYSTEM_FONT) { $env:GZ_SYSTEM_FONT = "$kyoeiFonts\NotoSans-Regular.ttf" }
if (-not $env:GZ_SYSTEM_FONT_JP) { $env:GZ_SYSTEM_FONT_JP = "$kyoeiFonts\NotoSansCJK-Regular.ttc" }
New-Item -ItemType Directory -Force -Path "$Work\logs" | Out-Null
$kyoeiLog = Join-Path "$Work\logs" ((Get-Date -Format 'yyyyMMdd_HHmmss')+'.log')
& "$kyoeiRoot\build\kyoei.exe" "$Work\out" --app0 $kyoeiProof.source --user "$Work\user" --log $kyoeiLog
Write-Host "Código de saída: $LASTEXITCODE; registro: $kyoeiLog"
exit $LASTEXITCODE
