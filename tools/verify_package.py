"""Run the rendered TC-029 packaged fixture and validate fresh output evidence."""
import argparse
import array
import json
import math
from pathlib import Path
import subprocess
import sys
import time
import wave

class EvidenceError(ValueError):
    pass

def inspect_evidence(saved, started, log_text, returncode):
    if returncode != 0:
        raise EvidenceError(f"Packaged process exited {returncode}")
    if any(word in log_text for word in ('Error:', 'Fatal error', 'Ensure condition', 'MS_SCENARIO FAIL')):
        raise EvidenceError('Package logged an error; inspect the scenario log')
    marker='MS_SCENARIO_RESULT='
    reports=[json.loads(line.split(marker,1)[1]) for line in log_text.splitlines() if marker in line]
    if not reports:
        raise EvidenceError('Current process did not emit its completion marker')
    report=reports[-1]
    if (report.get('status')!='passed' or report.get('checks',0)<25
            or report.get('corpses')!=2 or report.get('deployments')!=2):
        raise EvidenceError('Scenario did not pass all current checks')
    saved=Path(saved)
    for name in ('turret-scenario.json','turret-spawn.png','turret-hit.png','ragdoll-fall.png','ragdoll-two.png','turret-audio.wav'):
        p=saved/name
        if not p.is_file() or p.stat().st_mtime<started:
            raise EvidenceError(f'Missing or stale output: {name}')
    if json.loads((saved/'turret-scenario.json').read_text(encoding='utf-8-sig'))!=report:
        raise EvidenceError('Saved report does not match the current process')
    for name in ('turret-spawn.png','turret-hit.png','ragdoll-fall.png','ragdoll-two.png'):
        data=(saved/name).read_bytes()
        if len(data)<1000 or not data.startswith(b'\x89PNG\r\n\x1a\n'):
            raise EvidenceError(f'Invalid render capture: {name}')
    with wave.open(str(saved/'turret-audio.wav'),'rb') as sound:
        if sound.getsampwidth()!=2 or sound.getnframes()<sound.getframerate()*.5:
            raise EvidenceError('Audio is not a sufficient 16-bit output capture')
        samples=array.array('h',sound.readframes(sound.getnframes()))
    rms=math.sqrt(sum(x*x for x in samples)/max(len(samples),1))/32768
    peak=max((abs(x) for x in samples),default=0)/32768
    if rms<.0001 or peak<.001 or peak>=.99:
        raise EvidenceError(f'Silent or clipped output: RMS={rms}, peak={peak}')
    return {'status':'passed','scenario':report,'audio_rms':rms,'audio_peak':peak}

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    root=Path(__file__).resolve().parents[1]
    parser.add_argument('--package',type=Path,default=root/'.artifacts/TurretApproach/Windows')
    args=parser.parse_args()
    package=args.package.resolve()
    exe=package/'MeatShield/Binaries/Win64/MeatShield.exe'
    log_path=root/'.local/packaged-scenario.log'
    try:
        if not exe.is_file(): raise EvidenceError(f'Build the package first; missing {exe}')
        log_path.parent.mkdir(parents=True,exist_ok=True)
        started=time.time()
        command=[str(exe),'-MSScenario','-unattended','-nosplash','-windowed','-ResX=1600','-ResY=900','-stdout','-FullStdOutLogOutput','-UTF8Output']
        with log_path.open('w',encoding='utf-8') as log:
            result=subprocess.run(command,cwd=package,stdout=log,stderr=subprocess.STDOUT,timeout=120,shell=False)
        report=inspect_evidence(package/'MeatShield/Saved',started,log_path.read_text(encoding='utf-8',errors='replace'),result.returncode)
        report['log_path']=str(log_path)
        print(json.dumps(report,indent=2));return 0
    except (OSError,ValueError,wave.Error,subprocess.TimeoutExpired) as exc:
        print(f'Package verification failed: {exc}. Inspect {log_path}',file=sys.stderr);return 1

if __name__=='__main__':raise SystemExit(main())
