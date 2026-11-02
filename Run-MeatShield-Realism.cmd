@echo off
setlocal
set "GAME_EXE=%~dp0.artifacts\TurretApproach\Windows\MeatShield.exe"
if not exist "%GAME_EXE%" (
  echo Build the Windows package with tools\build.ps1 first.
  pause
  exit /b 1
)
start "Meat Shield - MetaHuman experiment" "%GAME_EXE%" -MSRealism -windowed -ResX=1600 -ResY=900 %*
