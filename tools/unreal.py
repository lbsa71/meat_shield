"""Launch the pinned Unreal editor through explicit environment boundaries."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import json
import os
from pathlib import Path
import subprocess
import sys

REPO_ROOT = Path(__file__).resolve().parents[1]


class ConfigurationError(ValueError):
    """The requested engine or project cannot satisfy the launch contract."""


@dataclass(frozen=True)
class LaunchPlan:
    argv: tuple[str, ...]
    environment: dict[str, str]
    repo_root: Path
    cache_root: Path
    log_path: Path
    mode: str

    def public_details(self):
        """Expose launch inputs without serializing inherited credentials."""
        return {"mode": self.mode, "argv": list(self.argv),
                "cwd": str(self.repo_root), "cache_root": str(self.cache_root),
                "log_path": str(self.log_path)}


def read_json(path):
    try:
        return json.loads(path.read_text(encoding="utf-8-sig"))
    except (OSError, ValueError) as exc:
        raise ConfigurationError(f"Cannot read configuration {path}: {exc}") from exc


def build_launch_plan(repo_root, engine_root=None, mode="smoke", environ=None,
                      project=None, cache_root=None):
    if mode not in {"smoke", "editor", "headless"}:
        raise ConfigurationError(f"Unknown launch mode: {mode}")
    repo_root = Path(repo_root).resolve()
    environment = dict(os.environ if environ is None else environ)
    configured_engine = engine_root if engine_root is not None else environment.get("MEAT_SHIELD_UNREAL_ROOT")
    if not configured_engine:
        raise ConfigurationError("Set MEAT_SHIELD_UNREAL_ROOT or pass --engine-root")
    engine_root = Path(configured_engine).expanduser().resolve()
    try:
        pin = read_json(repo_root / "planning/toolchain.json")["unreal"]
        installed = read_json(engine_root / "Engine/Build/Build.version")
        expected = tuple(pin[key] for key in ("major", "minor", "patch", "changelist"))
        actual = tuple(installed[key] for key in ("MajorVersion", "MinorVersion", "PatchVersion", "Changelist"))
    except (KeyError, TypeError) as exc:
        raise ConfigurationError(f"Invalid engine version metadata: {exc}") from exc
    if actual != expected:
        raise ConfigurationError(f"Engine version mismatch: expected {expected}, found {actual}")
    project = Path(project) if project is not None else Path("game/MeatShield.uproject")
    project = (repo_root / project).resolve()
    if not project.is_file():
        raise ConfigurationError(f"Missing project: {project}")
    name = "UnrealEditor.exe" if mode == "editor" else "UnrealEditor-Cmd.exe"
    executable = engine_root / "Engine/Binaries/Win64" / name
    if not executable.is_file():
        raise ConfigurationError(f"Missing engine executable: {executable}")
    cache_root = Path(cache_root) if cache_root is not None else Path(".local/unreal-ddc")
    cache_root = (repo_root / cache_root).resolve()
    environment["UE-LocalDataCachePath"] = str(cache_root)
    argv = [str(executable), str(project), "-nop4", "-nosplash", "-stdout", "-FullStdOutLogOutput", "-UTF8Output"]
    if mode == "smoke":
        script = repo_root / "tools/unreal_smoke.py"
        if not script.is_file():
            raise ConfigurationError(f"Missing smoke script: {script}")
        argv += ["-unattended", "-nullrhi", "-run=pythonscript", f"-script={script}"]
    else:
        argv += ["-ModelContextProtocolStartServer"]
        if mode == "headless":
            argv += ["-unattended", "-nullrhi", "-RenderOffScreen", "-nosound"]
    return LaunchPlan(tuple(argv), environment, repo_root, cache_root,
                      repo_root / ".local" / f"unreal-{mode}.log", mode)


def smoke_evidence(plan):
    pin = read_json(plan.repo_root / "planning/toolchain.json")["unreal"]
    expected = f"{pin['major']}.{pin['minor']}.{pin['patch']}-{pin['changelist']}"
    marker = "MEAT_SHIELD_ENGINE_SMOKE="
    for line in plan.log_path.read_text(encoding="utf-8", errors="replace").splitlines():
        if marker not in line:
            continue
        try:
            evidence = json.loads(line.split(marker, 1)[1])
        except ValueError:
            continue
        if (isinstance(evidence, dict) and evidence.get("python_commandlet") == "passed"
                and str(evidence.get("engine_version", "")).split("+", 1)[0] == expected):
            return evidence
    return None


def execute(plan):
    plan.cache_root.mkdir(parents=True, exist_ok=True)
    plan.log_path.parent.mkdir(parents=True, exist_ok=True)
    flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" and plan.mode != "editor" else 0
    with plan.log_path.open("w", encoding="utf-8") as log:
        arguments = {"cwd": str(plan.repo_root), "env": plan.environment,
                     "stdout": log, "stderr": subprocess.STDOUT,
                     "creationflags": flags, "shell": False}
        if plan.mode == "smoke":
            try:
                result = subprocess.run(plan.argv, timeout=300, **arguments)
            except subprocess.TimeoutExpired as exc:
                raise ConfigurationError(f"Engine smoke timed out; inspect {plan.log_path}") from exc
            evidence = smoke_evidence(plan)
            passed = result.returncode == 0 and evidence is not None
            return {"status": "passed" if passed else "failed",
                    "returncode": 0 if passed else (result.returncode or 1),
                    "evidence": evidence, **plan.public_details()}
        process = subprocess.Popen(plan.argv, **arguments)
        # A started process is not yet a verified editor or MCP connection.
        return {"status": "started", "pid": process.pid, **plan.public_details()}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=["smoke", "editor", "headless"])
    parser.add_argument("--engine-root", help="Unreal root, overriding MEAT_SHIELD_UNREAL_ROOT")
    parser.add_argument("--project", help="Project path, relative to the repo or absolute")
    parser.add_argument("--cache-root", help="Local cache directory, relative to the repo or absolute")
    parser.add_argument("--dry-run", action="store_true", help="Print the launch plan without starting or writing anything")
    args = parser.parse_args(argv)
    try:
        plan = build_launch_plan(REPO_ROOT, args.engine_root, args.mode,
                                 project=args.project, cache_root=args.cache_root)
        result = plan.public_details() if args.dry_run else execute(plan)
        print(json.dumps(result, indent=2))
        return result.get("returncode", 0)
    except (ConfigurationError, OSError) as exc:
        print(f"Unreal setup error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
