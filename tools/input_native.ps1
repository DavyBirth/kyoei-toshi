param([Parameter(Mandatory=$true)][int]$TargetId,
 [Parameter(Mandatory=$true)][ValidateSet('Enter','Space','Z','X','Q','Up','Down','Left','Right','W','A','S','D','I','J','K','L')][string]$Key,
 [ValidateRange(50,3000)][int]$HoldMs=180)
$ErrorActionPreference = 'Stop'
Add-Type @'
using System;
using System.Runtime.InteropServices;
public class KyoeiInput {
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h,uint msg,UIntPtr w,IntPtr l);
 [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint key,uint mode);
}
'@
$kyoeiProcess = Get-Process -Id $TargetId -ErrorAction Stop
$kyoeiExe = [IO.Path]::GetFullPath((Join-Path (Split-Path $PSScriptRoot) 'build/kyoei.exe'))
if ($kyoeiProcess.ProcessName -ne 'kyoei' -or $kyoeiProcess.Path -ne $kyoeiExe -or -not $kyoeiProcess.MainWindowHandle) { throw 'Janela diferente do port Kyoei' }
$kyoeiKeys = @{Enter=13; Space=32; Z=90; X=88; Q=81; Up=38; Down=40; Left=37; Right=39; W=87; A=65; S=83; D=68; I=73; J=74; K=75; L=76}
$kyoeiVirtual = [uint32]$kyoeiKeys[$Key]
$kyoeiScan = [KyoeiInput]::MapVirtualKey($kyoeiVirtual,0)
$kyoeiParam = [int64](1 -bor ($kyoeiScan -shl 16))
if ($Key -in @('Up','Down','Left','Right')) { $kyoeiParam = $kyoeiParam -bor 0x01000000 }
if (-not [KyoeiInput]::PostMessage($kyoeiProcess.MainWindowHandle,0x100,[UIntPtr]$kyoeiVirtual,[IntPtr]$kyoeiParam)) { throw 'Falha ao pressionar tecla' }
try { Start-Sleep -Milliseconds $HoldMs } finally {
 [KyoeiInput]::PostMessage($kyoeiProcess.MainWindowHandle,0x101,[UIntPtr]$kyoeiVirtual,[IntPtr]($kyoeiParam -bor 3221225472L)) | Out-Null
}
Write-Output "Kyoei: tecla $Key enviada somente ao processo $TargetId"
