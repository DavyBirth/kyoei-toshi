# Prepare the build/test environment only. Never restart Windows automatically.
$ErrorActionPreference='Stop'
$linuxProjectRoot=Split-Path $PSScriptRoot
$linuxStatusPath=Join-Path $linuxProjectRoot 'build/linux-setup-status.json'
function Save-LinuxStatus($State,$Detail) {
    @{state=$State;detail=$Detail;restart_performed=$false} | ConvertTo-Json | Set-Content -LiteralPath $linuxStatusPath -Encoding UTF8
}
try {
    Save-LinuxStatus 'checking_features' 'Checking Windows Linux/virtualization components.'
    $linuxNeedsRestart=$false
    foreach($linuxFeatureName in @('Microsoft-Windows-Subsystem-Linux','VirtualMachinePlatform')) {
        $linuxFeature=Get-WindowsOptionalFeature -Online -FeatureName $linuxFeatureName
        if($linuxFeature.State -eq 'EnablePending') {$linuxNeedsRestart=$true}
        elseif($linuxFeature.State -ne 'Enabled') {
            $linuxFeatureResult=Enable-WindowsOptionalFeature -Online -FeatureName $linuxFeatureName -All -NoRestart
            $linuxNeedsRestart=$linuxNeedsRestart -or $linuxFeatureResult.RestartNeeded
        }
    }
    if($linuxNeedsRestart) {
        Save-LinuxStatus 'restart_required' 'Windows components prepared. A user-initiated restart is required before installing Ubuntu. No restart was requested.'
        exit 0
    }
    Save-LinuxStatus 'installing_distribution' 'Installing Ubuntu for Linux compilation/testing; no automatic launch or restart.'
    & "$env:WINDIR\System32\wsl.exe" --install --no-launch --distribution Ubuntu *> (Join-Path $linuxProjectRoot 'build/linux-setup.log')
    if($LASTEXITCODE -ne 0) {throw "WSL installer exited with code $LASTEXITCODE; see build/linux-setup.log"}
    Save-LinuxStatus 'distribution_installed' 'Ubuntu installed. Initialization and toolchain setup remain to be verified.'
} catch {
    Save-LinuxStatus 'failed' $_.Exception.Message
    exit 1
}
