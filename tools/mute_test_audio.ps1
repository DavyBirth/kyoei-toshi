param([Parameter(Mandatory=$true)][int]$TargetId)
$ErrorActionPreference='Stop'
$expected=@('kyoei.exe','kyoei-gow3.exe') | ForEach-Object { Join-Path (Split-Path $PSScriptRoot -Parent) "build\$_" }
$process=Get-Process -Id $TargetId
if($process.Path -notin $expected){throw 'O alvo deve ser o runtime Kyoei deste projeto.'}
if(-not ('KyoeiTestAudio' -as [type])) {
Add-Type @'
using System;
using System.Runtime.InteropServices;
[ComImport,Guid("BCDE0395-E52F-467C-8E3D-C4579291692E")] class AudioEnumerator {}
[ComImport,Guid("A95664D2-9614-4F35-A746-DE8DB63617E6"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface IDevices {
 int EnumAudioEndpoints();
 [PreserveSig] int GetDefaultAudioEndpoint(int flow,int role,out IDevice device);
}
[ComImport,Guid("D666063F-1587-4E43-81F1-B948E807363F"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface IDevice {
 [PreserveSig] int Activate(ref Guid iid,uint context,IntPtr parameters,[MarshalAs(UnmanagedType.IUnknown)] out object result);
}
[ComImport,Guid("77AA99A0-1BD6-484F-8BC7-2C654C9A9B6F"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface ISessions {
 int GetAudioSessionControl(); int GetSimpleAudioVolume();
 [PreserveSig] int GetSessionEnumerator(out ISessionList sessions);
}
[ComImport,Guid("E2F5BB11-0570-40CA-ACDD-3AA01277DEE8"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface ISessionList {
 [PreserveSig] int GetCount(out int count);
 [PreserveSig] int GetSession(int index,[MarshalAs(UnmanagedType.IUnknown)] out object session);
}
[ComImport,Guid("BFB7FF88-7239-4FC9-8FA2-07C950BE9C6D"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface ISessionInfo {
 int GetState(); int GetDisplayName(); int SetDisplayName(); int GetIconPath(); int SetIconPath();
 int GetGroupingParam(); int SetGroupingParam(); int RegisterAudioSessionNotification(); int UnregisterAudioSessionNotification();
 int GetSessionIdentifier(); int GetSessionInstanceIdentifier();
 [PreserveSig] int GetProcessId(out uint id);
}
[ComImport,Guid("87CE5498-68D6-44E5-9215-6DA47EF883D8"),InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] interface IVolume {
 int SetMasterVolume(); int GetMasterVolume();
 [PreserveSig] int SetMute([MarshalAs(UnmanagedType.Bool)] bool mute,ref Guid context);
 [PreserveSig] int GetMute([MarshalAs(UnmanagedType.Bool)] out bool mute);
}
public static class KyoeiTestAudio {
 public static int Mute(uint target) {
  var devices=(IDevices)new AudioEnumerator(); IDevice device;
  Marshal.ThrowExceptionForHR(devices.GetDefaultAudioEndpoint(0,1,out device));
  object manager; var iid=typeof(ISessions).GUID;
  Marshal.ThrowExceptionForHR(device.Activate(ref iid,23,IntPtr.Zero,out manager));
  ISessionList sessions; Marshal.ThrowExceptionForHR(((ISessions)manager).GetSessionEnumerator(out sessions));
  int count; Marshal.ThrowExceptionForHR(sessions.GetCount(out count)); int muted=0;
  for(int i=0;i<count;i++) {
   object session; Marshal.ThrowExceptionForHR(sessions.GetSession(i,out session));
   uint id; Marshal.ThrowExceptionForHR(((ISessionInfo)session).GetProcessId(out id));
   if(id!=target)continue;
   var volume=(IVolume)session; var context=Guid.Empty;
   Marshal.ThrowExceptionForHR(volume.SetMute(true,ref context));
   bool value; Marshal.ThrowExceptionForHR(volume.GetMute(out value));
   if(!value)throw new Exception("A sessao nao ficou muda.");
   muted++;
  }
  return muted;
 }
}
'@
}
[KyoeiTestAudio]::Mute([uint32]$TargetId)
