param([Parameter(Mandatory=$true)][int]$TargetId,
 [Parameter(Mandatory=$true)][ValidateSet('Enter','Space','E','F','F8','F10','F12','Escape','Tab','Z','X','Q','Up','Down','Left','Right','W','A','S','D','I','J','K','L','H','M','R','G','V')][string]$Key,
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
$kyoeiExe = @('kyoei.exe','kyoei-gow3.exe') | ForEach-Object { [IO.Path]::GetFullPath((Join-Path (Split-Path $PSScriptRoot) "build/$_")) }
if ($kyoeiProcess.Path -notin $kyoeiExe -or -not $kyoeiProcess.MainWindowHandle) { throw 'Janela diferente do port Kyoei' }
$kyoeiKeys = @{Enter=13; Space=32; E=69; F=70; Escape=27; Tab=9; Z=90; X=88; Q=81; Up=38; Down=40; Left=37; Right=39; W=87; A=65; S=83; D=68; I=73; J=74; K=75; L=76; H=72; M=77; R=82; G=71; V=86}
$kyoeiKeys['F8']=119
$kyoeiKeys['F10']=121
$kyoeiKeys['F12']=123
$kyoeiVirtual = [uint32]$kyoeiKeys[$Key]
$kyoeiScan = [KyoeiInput]::MapVirtualKey($kyoeiVirtual,0)
$kyoeiParam = [int64](1 -bor ($kyoeiScan -shl 16))
if ($Key -in @('Up','Down','Left','Right')) { $kyoeiParam = $kyoeiParam -bor 0x01000000 }
if (-not [KyoeiInput]::PostMessage($kyoeiProcess.MainWindowHandle,0x100,[UIntPtr]$kyoeiVirtual,[IntPtr]$kyoeiParam)) { throw 'Falha ao pressionar tecla' }
try { Start-Sleep -Milliseconds $HoldMs } finally {
 [KyoeiInput]::PostMessage($kyoeiProcess.MainWindowHandle,0x101,[UIntPtr]$kyoeiVirtual,[IntPtr]($kyoeiParam -bor 3221225472L)) | Out-Null
}
Write-Output "Kyoei: tecla $Key enviada somente ao processo $TargetId"
