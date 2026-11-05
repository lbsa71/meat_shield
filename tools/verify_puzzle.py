"""Run TC-034 twice in the rendered package and compare replay evidence."""
import argparse
import json
from pathlib import Path
import subprocess
import time
from verify_package import EvidenceError

def inspect_puzzle(saved, started, log, code):
    if code or any(x in log for x in ('Error:', 'Fatal error', 'Ensure condition', 'MS_PUZZLE_CHECK FAIL')):
        raise EvidenceError('Puzzle process failed or logged a failed check/error')
    reports=[json.loads(x.split('MS_PUZZLE_RESULT=',1)[1]) for x in log.splitlines() if 'MS_PUZZLE_RESULT=' in x]
    if not reports: raise EvidenceError('Missing puzzle completion marker')
    r=reports[-1]
    if r.get('status')!='passed' or r.get('checks',0)<17 or r.get('failures')!=0 or r.get('route_waypoints')!=7:
        raise EvidenceError('Incomplete or failed puzzle checks')
    for name in ('puzzle-start.png','puzzle-single-cover.png','puzzle-route-end.png','puzzle-overview.png','puzzle-scenario.json'):
        p=Path(saved)/name
        if not p.is_file() or p.stat().st_mtime<started: raise EvidenceError(f'Missing/stale capture {name}')
        if name.endswith('.png') and (p.stat().st_size<1000 or not p.read_bytes().startswith(b'\x89PNG\r\n\x1a\n')): raise EvidenceError('Invalid screenshot')
    if json.loads((Path(saved)/'puzzle-scenario.json').read_text(encoding='utf-8-sig'))!=r: raise EvidenceError('Saved report differs from process marker')
    return r

def main():
    root=Path(__file__).resolve().parents[1]
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--package',type=Path,default=root/'.artifacts/TurretApproach/Windows')
    args=parser.parse_args(); package=args.package.resolve(); runs=[]
    for i in range(2):
        log=root/f'.local/puzzle-run-{i+1}.log'; started=time.time()
        command=[str(package/'MeatShield/Binaries/Win64/MeatShield.exe'),'-MSRealism','-MSPuzzle','-MSPuzzleScenario','-RenderOffscreen','-unattended','-nosplash','-windowed','-ResX=1280','-ResY=720','-stdout','-FullStdOutLogOutput','-UTF8Output']
        with log.open('w',encoding='utf-8') as output:
            p=subprocess.run(command,cwd=package,stdout=output,stderr=subprocess.STDOUT,timeout=180)
        runs.append(inspect_puzzle(package/'MeatShield/Saved',started,log.read_text(encoding='utf-8',errors='replace'),p.returncode))
    if runs[0]['layout_hash']!=runs[1]['layout_hash']: raise EvidenceError('Fresh launches produced different initial poses/collision')
    print(json.dumps({'status':'passed','runs':runs},indent=2))
if __name__=='__main__': main()
