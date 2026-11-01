"""MS-001; REQ-019 and REQ-021: launcher boundary and version checks."""

from pathlib import Path
from dataclasses import replace
from contextlib import redirect_stdout
import io
import json
import sys
import tempfile
import unittest
from unittest.mock import patch

from tools.unreal import ConfigurationError, build_launch_plan, execute, main


class UnrealLaunchTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="meat-shield-launch-tests-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name).resolve()
        # Cleanup owns only the directory allocated by TemporaryDirectory.
        self.assertTrue(self.root.is_relative_to(Path(tempfile.gettempdir()).resolve()))
        self.repo = self.root / "project with spaces"
        self.engine = self.root / "engine with spaces"
        (self.repo / "planning").mkdir(parents=True)
        (self.repo / "game").mkdir()
        (self.repo / "tools").mkdir()
        self.project = self.repo / "game/MeatShield.uproject"
        self.project.write_text('{"FileVersion": 3}', encoding="utf-8")
        (self.repo / "tools/unreal_smoke.py").touch()
        self.version = {"major": 5, "minor": 8, "patch": 4, "changelist": 58902052}
        (self.repo / "planning/toolchain.json").write_text(
            json.dumps({"unreal": self.version}), encoding="utf-8")
        (self.engine / "Engine/Build").mkdir(parents=True)
        (self.engine / "Engine/Binaries/Win64").mkdir(parents=True)
        self.write_engine_version()
        for name in ["UnrealEditor.exe", "UnrealEditor-Cmd.exe"]:
            (self.engine / "Engine/Binaries/Win64" / name).touch()

    def write_engine_version(self, patch=4, changelist=58902052):
        data = {"MajorVersion": 5, "MinorVersion": 8,
                "PatchVersion": patch, "Changelist": changelist}
        (self.engine / "Engine/Build/Build.version").write_text(
            json.dumps(data), encoding="utf-8")

    def plan(self, **kwargs):
        return build_launch_plan(self.repo, engine_root=self.engine, environ={}, **kwargs)

    def test_explicit_engine_overrides_environment_without_splitting_paths(self):
        plan = build_launch_plan(self.repo, engine_root=self.engine,
                                 environ={"MEAT_SHIELD_UNREAL_ROOT": "unusable-path"})
        self.assertEqual(Path(plan.argv[0]), self.engine / "Engine/Binaries/Win64/UnrealEditor-Cmd.exe")
        self.assertEqual(Path(plan.argv[1]), self.project)

    def test_engine_can_be_selected_from_environment(self):
        plan = build_launch_plan(self.repo, environ={"MEAT_SHIELD_UNREAL_ROOT": str(self.engine)})
        self.assertEqual(Path(plan.argv[0]).parents[3], self.engine)

    def test_missing_engine_configuration_is_actionable(self):
        with self.assertRaisesRegex(ConfigurationError, "MEAT_SHIELD_UNREAL_ROOT"):
            build_launch_plan(self.repo, environ={})

    def test_wrong_patch_is_rejected_before_launch(self):
        self.write_engine_version(patch=3)
        with self.assertRaisesRegex(ConfigurationError, "version"):
            self.plan()

    def test_different_build_of_same_patch_is_rejected(self):
        self.write_engine_version(changelist=1)
        with self.assertRaisesRegex(ConfigurationError, "version"):
            self.plan()

    def test_missing_project_is_rejected(self):
        with self.assertRaisesRegex(ConfigurationError, "project"):
            self.plan(project=self.repo / "missing.uproject")

    def test_missing_executable_is_rejected(self):
        (self.engine / "Engine/Binaries/Win64/UnrealEditor.exe").unlink()
        with self.assertRaisesRegex(ConfigurationError, "executable"):
            self.plan(mode="editor")

    def test_cache_override_is_process_local_and_preserves_caller_environment(self):
        original = {"MEAT_SHIELD_UNREAL_ROOT": str(self.engine), "PRIVATE_TOKEN": "not-for-output"}
        plan = build_launch_plan(self.repo, environ=original)
        self.assertEqual(plan.environment["UE-LocalDataCachePath"], str(self.repo / ".local/unreal-ddc"))
        self.assertNotIn("UE-LocalDataCachePath", original)
        self.assertEqual(plan.environment["PRIVATE_TOKEN"], original["PRIVATE_TOKEN"])
        self.assertFalse((self.repo / ".local").exists(), "Planning a launch must not write caches")

    def test_headless_editor_runs_mcp_without_rendering(self):
        plan = self.plan(mode="headless")
        self.assertIn("-ModelContextProtocolStartServer", plan.argv)
        self.assertIn("-nullrhi", plan.argv)
        self.assertNotIn("-run=pythonscript", plan.argv)

    def test_normal_editor_keeps_graphics_available(self):
        plan = self.plan(mode="editor")
        self.assertTrue(plan.argv[0].endswith("UnrealEditor.exe"))
        self.assertIn("-ModelContextProtocolStartServer", plan.argv)
        self.assertNotIn("-nullrhi", plan.argv)

    def test_smoke_uses_commandlet_and_does_not_leave_a_server_running(self):
        plan = self.plan(mode="smoke")
        self.assertIn("-run=pythonscript", plan.argv)
        self.assertIn("-unattended", plan.argv)
        self.assertNotIn("-ModelContextProtocolStartServer", plan.argv)
        script_args = [arg for arg in plan.argv if arg.startswith("-script=")]
        self.assertEqual(len(script_args), 1)
        self.assertEqual(Path(script_args[0].split("=", 1)[1]), self.repo / "tools/unreal_smoke.py")

    def test_custom_cache_root_is_respected(self):
        cache = self.root / "cache with spaces"
        plan = self.plan(cache_root=cache)
        self.assertEqual(plan.environment["UE-LocalDataCachePath"], str(cache))

    def test_public_plan_does_not_disclose_environment_secrets(self):
        plan = build_launch_plan(self.repo, engine_root=self.engine,
                                 environ={"PRIVATE_TOKEN": "not-for-output"})
        text = json.dumps(plan.public_details())
        self.assertNotIn("PRIVATE_TOKEN", text)
        self.assertNotIn("not-for-output", text)

    def test_unknown_mode_is_rejected(self):
        with self.assertRaisesRegex(ConfigurationError, "mode"):
            self.plan(mode="unknown")

    def test_zero_exit_without_engine_smoke_evidence_is_a_failure(self):
        plan = replace(self.plan(), argv=(sys.executable, "-c", "pass"))
        result = execute(plan)
        self.assertEqual(result["status"], "failed")
        self.assertNotEqual(result["returncode"], 0)

    def test_real_subprocess_marker_is_required_and_reported(self):
        evidence = {"engine_version": "5.8.4-58902052+++UE5+Release-5.8", "python_commandlet": "passed"}
        script = "print(" + repr("MEAT_SHIELD_ENGINE_SMOKE=" + json.dumps(evidence)) + ")"
        plan = replace(self.plan(), argv=(sys.executable, "-c", script))
        result = execute(plan)
        self.assertEqual(result["status"], "passed")
        self.assertEqual(result["evidence"], evidence)

    def test_marker_from_another_engine_version_is_rejected(self):
        evidence = {"engine_version": "5.8.3-1+++UE5", "python_commandlet": "passed"}
        script = "print(" + repr("MEAT_SHIELD_ENGINE_SMOKE=" + json.dumps(evidence)) + ")"
        result = execute(replace(self.plan(), argv=(sys.executable, "-c", script)))
        self.assertEqual(result["status"], "failed")

    def test_malformed_marker_is_rejected(self):
        script = "print('MEAT_SHIELD_ENGINE_SMOKE=not-json')"
        result = execute(replace(self.plan(), argv=(sys.executable, "-c", script)))
        self.assertEqual(result["status"], "failed")

    def test_nonzero_exit_cannot_be_hidden_by_a_valid_marker(self):
        evidence = {"engine_version": "5.8.4-58902052+++UE5+Release-5.8", "python_commandlet": "passed"}
        script = "import sys; print(" + repr("MEAT_SHIELD_ENGINE_SMOKE=" + json.dumps(evidence)) + "); sys.exit(2)"
        result = execute(replace(self.plan(), argv=(sys.executable, "-c", script)))
        self.assertEqual(result["status"], "failed")
        self.assertEqual(result["returncode"], 2)

    def test_dry_run_never_calls_process_adapter_or_prints_secrets(self):
        output = io.StringIO()
        with patch("tools.unreal.REPO_ROOT", self.repo), \
             patch.dict("os.environ", {"MEAT_SHIELD_UNREAL_ROOT": str(self.engine), "PRIVATE_TOKEN": "not-for-output"}), \
             patch("tools.unreal.execute") as run, redirect_stdout(output):
            self.assertEqual(main(["smoke", "--dry-run"]), 0)
        run.assert_not_called()
        self.assertNotIn("not-for-output", output.getvalue())
        self.assertFalse((self.repo / ".local").exists())


if __name__ == "__main__":
    unittest.main()
