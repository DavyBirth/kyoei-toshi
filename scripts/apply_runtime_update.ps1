param([string]$GamePath=(Split-Path $PSScriptRoot),[switch]$CheckOnly)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GamePath)
$package=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'update.json') -Raw | ConvertFrom-Json
if ($package.schema -ne 1 -or $package.platform -ne 'windows-x64') { throw 'Pacote incompatível.' }
$manifest=Get-Content -LiteralPath (Join-Path $root 'manifest.json') -Raw | ConvertFrom-Json
if ($manifest.title -ne $package.title -or $manifest.version -ne $package.version) { throw 'Versão do jogo incompatível.' }
# Fixed paths: update metadata never chooses a filesystem destination.
$target=Join-Path $root 'runtime/kyoei.exe'
$payload=Join-Path $PSScriptRoot 'kyoei.exe'
$hash=(Get-FileHash -LiteralPath $payload -Algorithm SHA256).Hash.ToLowerInvariant()
if ($hash -ne $package.sha256) { throw 'Pacote danificado; baixe novamente.' }
$installed=(Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash.ToLowerInvariant()
if ($installed -eq $hash) { Write-Output 'Esta atualização já está instalada.'; exit 0 }
if ($installed -notin @($package.accepted_sha256)) { throw 'Runtime diferente da versão esperada. Nenhum arquivo alterado.' }
if ($CheckOnly) { Write-Output 'Atualização compatível; nenhum arquivo alterado.'; exit 0 }
$data=Join-Path $root 'data'
New-Item -ItemType Directory -Force -Path $data | Out-Null
$guard=$null
$probe=$null
$temporary=Join-Path $root 'runtime/kyoei-update.tmp'
try {
    $guard=[IO.File]::Open((Join-Path $data 'session.lock'),'OpenOrCreate','ReadWrite','None')
    # Windows refuses exclusive write access to an executable still in use.
    $probe=[IO.File]::Open($target,'Open','ReadWrite','None')
    $probe.Dispose(); $probe=$null
    if (Test-Path -LiteralPath $temporary) { throw 'Existe uma atualização pendente; preserve kyoei-update.tmp para diagnóstico.' }
    $backupDir=Join-Path $data 'update-backups'
    New-Item -ItemType Directory -Force -Path $backupDir | Out-Null
    $backup=Join-Path $backupDir ($installed+'.exe')
    if (Test-Path -LiteralPath $backup) { throw 'Backup anterior já existe; preserve-o antes de reaplicar.' }
    Copy-Item -LiteralPath $payload -Destination $temporary
    if ((Get-FileHash -LiteralPath $temporary).Hash.ToLowerInvariant() -ne $hash) { throw 'Falha na cópia. Runtime anterior preservado.' }
    [IO.File]::Replace($temporary,$target,$backup)
    Write-Output 'Atualização aplicada. Saves e arquivos do jogo preservados.'
    Write-Output ('Backup do runtime anterior: '+$backup)
} finally {
    if ($probe) { $probe.Dispose() }
    if ($guard) { $guard.Dispose() }
}
