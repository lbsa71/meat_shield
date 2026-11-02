"""REQ-025 / TC-029: stale files, silent audio and error logs cannot pass a package."""
import array
import json
import math
import os
from pathlib import Path
import tempfile
import time
import unittest
import wave
from tools.verify_package import EvidenceError, inspect_evidence

class PackageEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory(prefix='meat-shield-evidence-')
        self.addCleanup(self.temp.cleanup)
        self.saved=Path(self.temp.name).resolve()
        self.assertTrue(self.saved.is_relative_to(Path(tempfile.gettempdir()).resolve()))
        self.started=time.time()-10
        self.report={'status':'passed','checks':12,'shots':7,'hits':7,'deployments':1,'actors':25}
        (self.saved/'turret-scenario.json').write_text(json.dumps(self.report))
        self.log='MS_SCENARIO_RESULT='+json.dumps(self.report)
        for name in ['turret-spawn.png','turret-hit.png']:
            (self.saved/name).write_bytes(b'\x89PNG\r\n\x1a\n'+bytes(2000))
        self.audio()

    def audio(self,silent=False,clipped=False):
        samples=array.array('h',(0 if silent else 32767 if clipped else int(3000*math.sin(i*.1)) for i in range(24000)))
        with wave.open(str(self.saved/'turret-audio.wav'),'wb') as f:
            f.setparams((1,2,24000,0,'NONE','not compressed'));f.writeframes(samples.tobytes())

    def inspect(self,**kwargs):
        return inspect_evidence(self.saved,self.started,kwargs.get('log',self.log),kwargs.get('returncode',0))

    def test_complete_fresh_evidence_passes_and_reports_audio_energy(self):
        result=self.inspect();self.assertEqual(result['status'],'passed');self.assertGreater(result['audio_rms'],.001)
    def test_nonzero_process_exit_fails_even_with_valid_report(self):
        with self.assertRaises(EvidenceError):self.inspect(returncode=1)
    def test_missing_marker_cannot_pass_using_existing_report(self):
        with self.assertRaises(EvidenceError):self.inspect(log='normal startup only')
    def test_error_log_cannot_be_hidden_by_passing_fixture(self):
        with self.assertRaises(EvidenceError):self.inspect(log='LogOutputDevice: Error: asset class missing\n'+self.log)
    def test_stale_audio_cannot_pass(self):
        p=self.saved/'turret-audio.wav';os.utime(p,(self.started-1,self.started-1))
        with self.assertRaises(EvidenceError):self.inspect()
    def test_silent_output_fails(self):
        self.audio(silent=True)
        with self.assertRaises(EvidenceError):self.inspect()
    def test_clipped_output_fails(self):
        self.audio(clipped=True)
        with self.assertRaises(EvidenceError):self.inspect()
    def test_missing_render_capture_fails(self):
        (self.saved/'turret-hit.png').unlink()
        with self.assertRaises(EvidenceError):self.inspect()
    def test_old_partial_scenario_cannot_satisfy_current_contract(self):
        self.report['checks']=7;(self.saved/'turret-scenario.json').write_text(json.dumps(self.report))
        with self.assertRaises(EvidenceError):self.inspect(log='MS_SCENARIO_RESULT='+json.dumps(self.report))
    def test_report_disagrees_with_current_process(self):
        self.report['status']='failed';(self.saved/'turret-scenario.json').write_text(json.dumps(self.report))
        with self.assertRaises(EvidenceError):self.inspect()

if __name__=='__main__':unittest.main()
