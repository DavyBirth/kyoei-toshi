param([Parameter(Mandatory=$true)][int]$TargetId, [Parameter(Mandatory=$true)][string]$Out, [switch]$Focus)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
Add-Type @'
using System;
using System.Runtime.InteropServices;
public class KyoeiWindow {
 [DllImport("user32.dll")] public static extern IntPtr SetThreadDpiAwarenessContext(IntPtr context);
 [StructLayout(LayoutKind.Sequential)] public struct Rect { public int L,T,R,B; }
 [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h,out Rect r);
 [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h,IntPtr dc,uint flags);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern bool SetWindowText(IntPtr h,string text);
 [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h,int command);
 [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
}
'@
$kyoeiProcess = Get-Process -Id $TargetId -ErrorAction Stop
$kyoeiExe = [IO.Path]::GetFullPath((Join-Path (Split-Path $PSScriptRoot) 'build/kyoei.exe'))
if ($kyoeiProcess.ProcessName -ne 'kyoei' -or $kyoeiProcess.Path -ne $kyoeiExe) { throw 'Processo diferente do port Kyoei' }
$kyoeiWindow = $kyoeiProcess.MainWindowHandle
if (-not $kyoeiWindow) { throw 'Sem janela' }
# PrintWindow writes physical pixels; use the same units for GetWindowRect.
[KyoeiWindow]::SetThreadDpiAwarenessContext([IntPtr](-4)) | Out-Null
if ($Focus) {
 [KyoeiWindow]::ShowWindow($kyoeiWindow,9) | Out-Null
 [KyoeiWindow]::SetForegroundWindow($kyoeiWindow) | Out-Null
}
if ([KyoeiWindow]::IsIconic($kyoeiWindow)) { throw 'Janela minimizada: use -Focus para restaurar antes da captura' }
$kyoeiRect = New-Object KyoeiWindow+Rect
if (-not [KyoeiWindow]::GetWindowRect($kyoeiWindow,[ref]$kyoeiRect)) { throw 'Sem retangulo da janela' }
if (Test-Path -LiteralPath $Out) { throw 'Saida ja existe' }
$kyoeiImage = New-Object System.Drawing.Bitmap(($kyoeiRect.R-$kyoeiRect.L),($kyoeiRect.B-$kyoeiRect.T))
$kyoeiGraphics = [System.Drawing.Graphics]::FromImage($kyoeiImage)
$kyoeiDC = $kyoeiGraphics.GetHdc()
try {
 if (-not [KyoeiWindow]::PrintWindow($kyoeiWindow,$kyoeiDC,2)) { throw 'Captura falhou' }
} finally { $kyoeiGraphics.ReleaseHdc($kyoeiDC); $kyoeiGraphics.Dispose() }
try { $kyoeiImage.Save($Out,[System.Drawing.Imaging.ImageFormat]::Png) } finally { $kyoeiImage.Dispose() }
