# M1: merge the base game with the 01.01 update and prepare the loader files.
# Only reads the original game folders; everything is written under $Work (outside the repo).
param(
    [string]$Base  = 'G:\GodzillaShadPS4PC\launcher\game_data\CUSA02046',
    [string]$Patch = 'G:\GodzillaShadPS4PC\launcher\game_data\CUSA02046-patch',
    [string]$Work  = 'G:\godzilla-work-codex'
)
$ErrorActionPreference = 'Stop'
$scripts = $PSScriptRoot
$game = Join-Path $Work 'CUSA02046'
$out = Join-Path $Work 'out'
foreach ($step in @(
        @('merge_game.py', $Base, $Patch, $game),
        @('prepare.py', $game, '--out', $out),
        @('link_modules.py', $game, '--out', $out, '--firmware', 'G:\GodzillaShadPS4PC\user\sys_modules'),
        @('content_profile.py', $game, '--out', $out))) {
    python (Join-Path $scripts $step[0]) @($step[1..($step.Count - 1)])
    if ($LASTEXITCODE) { throw "$($step[0]) failed (exit $LASTEXITCODE)" }
}
