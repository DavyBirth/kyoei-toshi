$ErrorActionPreference = 'Stop'
$kyoeiRoot = Split-Path $PSScriptRoot
$kyoeiCompiler = Join-Path $env:WINDIR 'Microsoft.NET\Framework64\v4.0.30319\csc.exe'
if (-not (Test-Path -LiteralPath $kyoeiCompiler)) { throw 'Compilador .NET Framework local ausente' }
$kyoeiOutput = Join-Path $kyoeiRoot 'City Shrouded in Shadow.exe'
& $kyoeiCompiler /nologo /target:winexe /platform:x64 /optimize+ /r:System.Windows.Forms.dll "/out:$kyoeiOutput" "$kyoeiRoot\src\kyoei_launcher.cs"
if ($LASTEXITCODE -ne 0) { throw 'Falha ao compilar o launcher' }
Write-Output "Launcher compilado: $kyoeiOutput"
