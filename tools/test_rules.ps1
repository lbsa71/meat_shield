param()
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$vsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$install = & $vsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $install) { throw 'Install Visual Studio C++ Build Tools before running rule tests.' }
$devCmd = Join-Path $install 'Common7\Tools\VsDevCmd.bat'
$output = Join-Path $repoRoot '.local\rules-tests'
New-Item -ItemType Directory -Force -Path $output | Out-Null
$rules = Join-Path $repoRoot 'game\Source\MeatShield\Rules'
$test = Join-Path $repoRoot 'tools\tests\turret_rules_test.cpp'
$exe = Join-Path $output 'turret_rules_test.exe'
Push-Location $output
try {
    $batch = Join-Path $output 'compile.cmd'
    @"
@call "$devCmd" -no_logo -arch=x64 -host_arch=x64
@if errorlevel 1 exit /b 1
@cl.exe /nologo /std:c++20 /EHsc /W4 /I"$rules" "$test" "$rules\TurretRules.cpp" /Fe:"$exe"
@exit /b %ERRORLEVEL%
"@ | Set-Content -LiteralPath $batch -Encoding ascii
    & cmd.exe /d /c $batch
    if ($LASTEXITCODE -ne 0) { throw 'C++ rule test compilation failed (environment/build blocker).' }
    & $exe
    if ($LASTEXITCODE -ne 0) { throw 'C++ rule tests failed.' }
} finally { Pop-Location }
