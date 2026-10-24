param([string]$GamePath=(Split-Path $PSScriptRoot),[switch]$CheckOnly)
# Installs the small translation update: copies the listed files, then deletes
# the old 6 GB English archive. Saves, game/ and settings are never touched.
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GamePath)
function Hash($path) { (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() }
function Inside($name) {
    if ($name -match '(^[\\/])|(\.\.)|:') { throw 'Caminho inválido no pacote.' }
    $full=[IO.Path]::GetFullPath((Join-Path $root $name))
    if (-not $full.StartsWith($root.TrimEnd('\')+'\')) { throw 'Caminho fora da pasta do jogo.' }
    $full
}
$package=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'translation-update.json') -Raw | ConvertFrom-Json
if ($package.schema -ne 2) { throw 'Pacote incompatível.' }
$manifest=Get-Content -LiteralPath (Join-Path $root 'manifest.json') -Raw | ConvertFrom-Json
if ($manifest.title -ne $package.title -or $manifest.version -ne $package.version) { throw 'Versão do jogo incompatível.' }
$disc=Inside 'game/DAT/common.psarc'
if (-not (Test-Path -LiteralPath $disc) -or (Get-Item -LiteralPath $disc).Length -ne $package.disc_archive_bytes) { throw 'game/DAT/common.psarc não é o original do disco. Nenhum arquivo alterado.' }
$runtimeRow=$package.files | Where-Object { $_.path -eq 'runtime/kyoei.exe' }
$runtime=Hash (Inside 'runtime/kyoei.exe')
if ($runtime -ne $runtimeRow.sha256 -and $runtime -notin @($package.accepted_runtime_sha256)) { throw 'Runtime diferente das versões esperadas. Nenhum arquivo alterado.' }
$changes=@()
foreach ($row in $package.files) {
    $payload=Join-Path $PSScriptRoot ('payload/'+$row.path)
    if ((Hash $payload) -ne $row.sha256) { throw ('Pacote danificado: '+$row.path) }
    $target=Inside $row.path
    if (-not (Test-Path -LiteralPath $target) -or (Hash $target) -ne $row.sha256) { $changes+=$row }
}
$removals=@($package.remove | Where-Object { Test-Path -LiteralPath (Inside $_) })
if (-not $changes -and -not $removals) { Write-Output 'Esta atualização já está instalada.'; exit 0 }
foreach ($name in 'runtime/kyoei.exe','City Shrouded in Shadow.exe') {
    $probe=[IO.File]::Open((Inside $name),'Open','ReadWrite','None'); $probe.Dispose()
}
if ($CheckOnly) { Write-Output ('Atualização compatível; '+$changes.Count+' arquivos novos, '+$removals.Count+' antigos a excluir. Nenhum arquivo alterado.'); exit 0 }
$data=Join-Path $root 'data'
New-Item -ItemType Directory -Force -Path $data | Out-Null
$guard=[IO.File]::Open((Join-Path $data 'session.lock'),'OpenOrCreate','ReadWrite','None')
$backup=Join-Path $data ('update-backups/'+$package.release+'-'+(Get-Date -Format 'yyyyMMdd-HHmmss'))
$done=@()
try {
    foreach ($row in $changes) {
        $target=Inside $row.path; $temporary=$target+'.update-tmp'
        New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null
        Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('payload/'+$row.path)) -Destination $temporary -Force
        if ((Hash $temporary) -ne $row.sha256) { throw ('Falha na cópia: '+$row.path) }
    }
    foreach ($row in $changes) {
        $target=Inside $row.path; $saved=Join-Path $backup $row.path
        if (Test-Path -LiteralPath $target) {
            New-Item -ItemType Directory -Force -Path (Split-Path $saved) | Out-Null
            Move-Item -LiteralPath $target -Destination $saved
        }
        Move-Item -LiteralPath ($target+'.update-tmp') -Destination $target
        $done+=$row
    }
} catch {
    # Put back every file already replaced; nothing old has been deleted yet.
    foreach ($row in $done) {
        $target=Inside $row.path; $saved=Join-Path $backup $row.path
        Remove-Item -LiteralPath $target -Force
        if (Test-Path -LiteralPath $saved) { Move-Item -LiteralPath $saved -Destination $target }
    }
    foreach ($row in $changes) { Remove-Item -LiteralPath ((Inside $row.path)+'.update-tmp') -Force -ErrorAction SilentlyContinue }
    $guard.Dispose()
    throw ('Atualização desfeita, arquivos anteriores restaurados: '+$_)
}
# Only after every new file is in place: the old copy is no longer read.
foreach ($name in $removals) { Remove-Item -LiteralPath (Inside $name) -Force }
$guard.Dispose()
Write-Output 'Tradução atualizada. Saves e arquivos do jogo preservados.'
if ($removals) { Write-Output 'Tradução antiga de 6 GB excluída.' }
if (Test-Path -LiteralPath $backup) { Write-Output ('Arquivos pequenos anteriores guardados em: '+$backup) }
