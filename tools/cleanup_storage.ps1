# Execute only the reviewed, fixed-scope cleanup plan. Never traverse the dump.
$ErrorActionPreference = 'Stop'
$kyoeiWork = [IO.Path]::GetFullPath('G:\kyoei-toshi-work')
$kyoeiPlan = Get-Content -LiteralPath "$kyoeiWork\cleanup-20261009\plan.json" -Raw | ConvertFrom-Json
$kyoeiAllowedDirectories = @(
 "$kyoeiWork\m2\prova-01\CUSA03562",
 "$kyoeiWork\native-01\out", "$kyoeiWork\native-02\out", "$kyoeiWork\native-03\out"
)
$kyoeiMedia = @('.tex','.mdl','.mpk','.awb','.acb','.usm','.pak','.acb_tmp','.mp4')
function Assert-KyoeiPath([string]$Path) {
 $kyoeiAbsolute = [IO.Path]::GetFullPath($Path)
 $kyoeiResolved = (Resolve-Path -LiteralPath $Path -ErrorAction Stop).ProviderPath
 if ($kyoeiAbsolute -ne $kyoeiResolved -or -not $kyoeiResolved.StartsWith("$kyoeiWork\",[StringComparison]::OrdinalIgnoreCase)) { throw "Caminho fora do escopo: $Path" }
 $kyoeiCheck = Get-Item -LiteralPath $kyoeiResolved -Force
 while ($kyoeiCheck.FullName -ne $kyoeiWork) {
  if ($kyoeiCheck.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Link proibido: $Path" }
  if ($kyoeiCheck -is [IO.DirectoryInfo]) { $kyoeiCheck = $kyoeiCheck.Parent } else { $kyoeiCheck = $kyoeiCheck.Directory }
  if (-not $kyoeiCheck) { throw 'Raiz inesperada' }
 }
 return $kyoeiResolved
}
# Validate every target before deleting the first file.
foreach ($kyoeiRow in $kyoeiPlan.delete_files) {
 $kyoeiTarget = Assert-KyoeiPath $kyoeiRow.path
 if (-not $kyoeiTarget.StartsWith("$kyoeiWork\m1\",[StringComparison]::OrdinalIgnoreCase) -or [IO.Path]::GetExtension($kyoeiTarget) -notin $kyoeiMedia) { throw 'Arquivo fora da selecao de midia' }
 if ((Get-Item -LiteralPath $kyoeiTarget).Length -ne $kyoeiRow.bytes) { throw 'Arquivo mudou desde o plano' }
}
foreach ($kyoeiRow in $kyoeiPlan.delete_directories) {
 $kyoeiTarget = Assert-KyoeiPath $kyoeiRow.path
 if ($kyoeiTarget -notin $kyoeiAllowedDirectories) { throw 'Pasta fora da lista fixa' }
 foreach ($kyoeiChild in Get-ChildItem -LiteralPath $kyoeiTarget -Force -Recurse) {
  if ($kyoeiChild.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Link dentro da pasta' }
 }
}
$kyoeiDeleted = 0
foreach ($kyoeiRow in $kyoeiPlan.delete_files) {
 Remove-Item -LiteralPath $kyoeiRow.path -Force
 $kyoeiDeleted++
 if ($kyoeiDeleted % 5000 -eq 0) { Write-Output "Arquivos de midia removidos: $kyoeiDeleted" }
}
foreach ($kyoeiRow in $kyoeiPlan.delete_directories) {
 Remove-Item -LiteralPath $kyoeiRow.path -Recurse -Force
 Write-Output "Removido: $($kyoeiRow.path)"
}
[ordered]@{date=(Get-Date -Format o); files=$kyoeiDeleted; bytes=$kyoeiPlan.planned_bytes; directories=$kyoeiPlan.delete_directories} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath "$kyoeiWork\cleanup-20261009\completed.json" -Encoding UTF8
