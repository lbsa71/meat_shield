"""REQ-019: run inside Unreal's Python commandlet, not system Python."""

import json
from pathlib import Path
import unreal

version = unreal.SystemLibrary.get_engine_version()
pin = json.loads((Path(__file__).resolve().parents[1] / "planning/toolchain.json").read_text(encoding="utf-8"))["unreal"]
expected = f"{pin['major']}.{pin['minor']}.{pin['patch']}-{pin['changelist']}"
if version.split("+", 1)[0] != expected:
    raise RuntimeError(f"Unexpected running engine version: {version}")
print("MEAT_SHIELD_ENGINE_SMOKE=" + json.dumps({"engine_version": version, "python_commandlet": "passed"}))
