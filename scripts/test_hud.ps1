# User-run diagnostic launch; never invoked for gameplay by the agent.
param([switch]$CheckOnly,[switch]$NativeDialogs,[switch]$FitHud)
$ErrorActionPreference='Stop'
$kyoeiArguments=@{Language='en';Width=1280;Height=720;Diagnostics=$true;CheckOnly=$CheckOnly}
if ($FitHud) { $kyoeiArguments.FitHud=$true }
if (-not $NativeDialogs) { $kyoeiArguments.InGameDialogs=$true }
& "$PSScriptRoot\run_native.ps1" @kyoeiArguments
exit $LASTEXITCODE
