import sys
import unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from verify_puzzle import inspect_puzzle, EvidenceError
class PuzzleEvidenceTests(unittest.TestCase):
    def test_missing_completion_rejected(self):
        with self.assertRaises(EvidenceError): inspect_puzzle(Path('.'),0,'',0)
    def test_failed_process_rejected(self):
        with self.assertRaises(EvidenceError): inspect_puzzle(Path('.'),0,'',2)
    def test_failed_check_rejected(self):
        with self.assertRaises(EvidenceError): inspect_puzzle(Path('.'),0,'MS_PUZZLE_CHECK FAIL one_body',0)
