param([switch]$Watch, [ValidateRange(30,3600)][int]$IntervalSeconds=600,
      [ValidateRange(1,99)][double]$ReservePercent=15)
$ErrorActionPreference='Stop'
$usageRoot=Join-Path (Split-Path $PSScriptRoot) 'build\overnight'
New-Item -ItemType Directory -Force -Path $usageRoot | Out-Null
do {
    $anchor=Join-Path $usageRoot 'current-session-path.txt'
    $latestSession=$null
    if(Test-Path -LiteralPath $anchor) {
        $anchoredPath=(Get-Content -LiteralPath $anchor -Raw).Trim()
        if(Test-Path -LiteralPath $anchoredPath){$latestSession=Get-Item -LiteralPath $anchoredPath}
    }
    $newest=Get-ChildItem -LiteralPath (Join-Path $env:USERPROFILE '.codex\sessions') -Recurse -File -Filter '*.jsonl' |
        Sort-Object CreationTime -Descending | Select-Object -First 1
    if(-not $latestSession -or ($newest -and $newest.CreationTime -gt $latestSession.CreationTime)) {
        # Codex may rotate the rollout while this conversation continues.
        $latestSession=$newest
        if($latestSession){$latestSession.FullName | Set-Content -LiteralPath $anchor -Encoding UTF8}
    }
    $sample=$null
    if($latestSession) {
        Get-Content -LiteralPath $latestSession.FullName -Tail 600 | ForEach-Object {
            if($_ -like '*"token_count"*') {
                try {
                    $entry=$_ | ConvertFrom-Json
                    if($entry.type -eq 'event_msg' -and $entry.payload.type -eq 'token_count' -and $entry.payload.rate_limits.primary) {
                        $sample=[ordered]@{checked_at=(Get-Date).ToUniversalTime().ToString('o'); observed_at=$entry.timestamp;
                            primary_remaining=100-[double]$entry.payload.rate_limits.primary.used_percent;
                            weekly_remaining=100-[double]$entry.payload.rate_limits.secondary.used_percent;
                            primary_resets_at=$entry.payload.rate_limits.primary.resets_at;
                            weekly_resets_at=$entry.payload.rate_limits.secondary.resets_at}
                    }
                } catch {}
            }
        }
    }
    if($sample) {
        $sample['stale']=((Get-Date).ToUniversalTime()-[DateTime]::Parse($sample.observed_at).ToUniversalTime()).TotalMinutes -gt 20
        $sample['finish_requested']=($sample.primary_remaining -le $ReservePercent -or $sample.weekly_remaining -le $ReservePercent)
        $payload=$sample | ConvertTo-Json -Compress
        $payload | Set-Content -LiteralPath (Join-Path $usageRoot 'usage-current.json') -Encoding UTF8
        Add-Content -LiteralPath (Join-Path $usageRoot 'usage-history.jsonl') -Encoding UTF8 -Value $payload
        if(-not $Watch) { Write-Output $payload }
    }
    if(-not $Watch) { break }
    Start-Sleep -Seconds $IntervalSeconds
} while(-not (Test-Path -LiteralPath (Join-Path $usageRoot 'stop-monitor')))
