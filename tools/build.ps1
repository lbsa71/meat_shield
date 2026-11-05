param(
    [ValidateSet('Editor','Package')][string]$Target = 'Package',
    [string]$EngineRoot = $env:MEAT_SHIELD_UNREAL_ROOT
)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
if (-not $EngineRoot) { throw 'Set MEAT_SHIELD_UNREAL_ROOT or pass -EngineRoot.' }
$EngineRoot = (Resolve-Path -LiteralPath $EngineRoot).Path
$pin = Get-Content -LiteralPath (Join-Path $repoRoot 'planning/toolchain.json') -Raw | ConvertFrom-Json
$installed = Get-Content -LiteralPath (Join-Path $EngineRoot 'Engine/Build/Build.version') -Raw | ConvertFrom-Json
if ($installed.MajorVersion -ne $pin.unreal.major -or $installed.MinorVersion -ne $pin.unreal.minor -or $installed.PatchVersion -ne $pin.unreal.patch -or $installed.Changelist -ne $pin.unreal.changelist) { throw 'Engine does not match planning/toolchain.json.' }
$project = Join-Path $repoRoot 'game/MeatShield.uproject'
$archive = Join-Path $repoRoot '.artifacts/TurretApproach'
$localData = Join-Path $repoRoot '.local'
New-Item -ItemType Directory -Force -Path $localData | Out-Null
$oldCache = [Environment]::GetEnvironmentVariable('UE-LocalDataCachePath','Process')
[Environment]::SetEnvironmentVariable('UE-LocalDataCachePath',(Join-Path $localData 'unreal-ddc'),'Process')
try {
    & (Join-Path $EngineRoot 'Engine/Build/BatchFiles/Build.bat') MeatShieldEditor Win64 Development $project -WaitMutex -NoHotReloadFromIDE -NoUBA -MaxParallelActions=6 2>&1 | Tee-Object -FilePath (Join-Path $localData 'build-editor.log')
    if ($LASTEXITCODE -ne 0) { throw 'Editor build failed. Inspect .local/build-editor.log.' }
    if ($Target -eq 'Package') {
        & (Join-Path $EngineRoot 'Engine/Build/BatchFiles/RunUAT.bat') BuildCookRun "-project=$project" -target=MeatShield -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -iostore -archive "-archivedirectory=$archive" -prereqs -utf8output -unattended -NoCompileEditor '-UbtArgs=-NoUBA -MaxParallelActions=6' 2>&1 | Tee-Object -FilePath (Join-Path $localData 'package.log')
        if ($LASTEXITCODE -ne 0) { throw 'Packaging failed. Inspect .local/package.log.' }
        $exe = Join-Path $archive 'Windows/MeatShield.exe'
        if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw "Package exited without expected executable: $exe" }
        $launcher = @'
@echo off
setlocal
start "Meat Shield" "%~dp0MeatShield.exe" -MSRealism -MSPuzzle -windowed -ResX=1600 -ResY=900 %*
'@
        Set-Content -LiteralPath (Join-Path $archive 'Windows/Run-MeatShield.cmd') -Value $launcher -Encoding ascii
        Write-Output "Runnable build: $exe"
    }
} finally { [Environment]::SetEnvironmentVariable('UE-LocalDataCachePath',$oldCache,'Process') }
