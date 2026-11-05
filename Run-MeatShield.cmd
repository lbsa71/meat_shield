@echo off
setlocal
set "GAME_EXE=%~dp0.artifacts\TurretApproach\Windows\MeatShield.exe"
if not exist "%GAME_EXE%" (
  echo No packaged build found. See docs\engineering\running-the-prototype.md.
  pause
  exit /b 1
)
start "Meat Shield" "%GAME_EXE%" -MSRealism -MSPuzzle -windowed -ResX=1600 -ResY=900
