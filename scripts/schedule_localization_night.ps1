$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot
$shutdownAt = [datetime]'2026-10-14T03:00:00'
if ((Get-Date) -ge $shutdownAt) { throw 'The authorized shutdown time has passed; do not schedule another day.' }
$service = New-Object -ComObject 'Schedule.Service'
$service.Connect()
$folder = $service.GetFolder('\')
$identity = [Security.Principal.WindowsIdentity]::GetCurrent().Name
$scriptPath = Join-Path $projectRoot 'scripts\localization_shutdown.ps1'
foreach ($kind in @('Checkpoint','Shutdown')) {
    $definition = $service.NewTask(0)
    $definition.RegistrationInfo.Description = 'Davy authorized localization checkpoints and shutdown at 03:00 on 2026-10-14.'
    $definition.Principal.UserId = $identity
    $definition.Principal.LogonType = 3
    $definition.Settings.Enabled = $true
    $definition.Settings.Hidden = $true
    $definition.Settings.DisallowStartIfOnBatteries = $false
    $definition.Settings.StopIfGoingOnBatteries = $false
    $definition.Settings.StartWhenAvailable = $false
    $definition.Settings.WakeToRun = ($kind -eq 'Shutdown')
    $definition.Settings.MultipleInstances = 2
    $definition.Settings.ExecutionTimeLimit = 'PT3M'
    $trigger = $definition.Triggers.Create(1)
    if ($kind -eq 'Checkpoint') {
        $trigger.StartBoundary = (Get-Date).AddMinutes(5).ToString('yyyy-MM-ddTHH:mm:ss')
        $trigger.EndBoundary = $shutdownAt.ToString('yyyy-MM-ddTHH:mm:ss')
        $trigger.Repetition.Interval = 'PT5M'
    } else {
        $trigger.StartBoundary = $shutdownAt.ToString('yyyy-MM-ddTHH:mm:ss')
        $trigger.EndBoundary = $shutdownAt.AddMinutes(5).ToString('yyyy-MM-ddTHH:mm:ss')
    }
    $action = $definition.Actions.Create(0)
    $action.Path = "$env:SystemRoot\System32\WindowsPowerShell\v1.0\powershell.exe"
    $action.WorkingDirectory = $projectRoot
    $action.Arguments = '-NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File "' + $scriptPath + '"'
    if ($kind -eq 'Checkpoint') { $action.Arguments += ' -CheckpointOnly' }
    $taskName = "KyoeiLocalization$kind-20261014"
    $task = $folder.RegisterTaskDefinition($taskName,$definition,6,$identity,$null,3)
    [pscustomobject]@{Name=$task.Name;NextRunTime=$task.NextRunTime;Enabled=$task.Enabled}
}
