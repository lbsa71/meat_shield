"""Local, editor-only recipe dispatcher for APIs absent from the MCP connector.

Run once in Unreal Python. Requests/results stay under ignored .local; no socket,
generic code input, credentials, or packaged runtime dependency is introduced.
"""
import json
import time
import traceback
from pathlib import Path
import unreal


class RecipeQueue:
    recipes = {name: name + ".py" for name in (
        "prepare_metahuman", "retry_metahuman_textures", "assemble_metahuman",
        "create_realism_assets", "inspect_realism_assets", "save_realism_assets")}

    def __init__(self):
        self.root = Path(unreal.Paths.project_dir()).resolve().parent
        self.local = self.root / ".local"
        self.last_id = None
        self.busy = False
        self.next_poll = 0
        self.handle = None

    def tick(self, _delta):
        if self.busy or time.monotonic() < self.next_poll:
            return
        self.next_poll = time.monotonic() + .25
        request = self.local / "editor-request.json"
        if not request.is_file():
            return
        try:
            data = json.loads(request.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            return
        if not isinstance(data.get("id"), str) or data["id"] == self.last_id:
            return
        self.last_id = data["id"]
        self.busy = True
        if self.handle is not None:
            unreal.unregister_slate_post_tick_callback(self.handle)
            self.handle = None
        result = {"id": self.last_id, "action": data.get("action")}
        try:
            action = data.get("action")
            if action not in self.recipes:
                raise ValueError("Unknown recipe; only the versioned allowlist may execute")
            script = self.root / "tools" / self.recipes[action]
            namespace = {"__name__": "__main__", "__file__": str(script), "RESULT": {}}
            exec(compile(script.read_text(encoding="utf-8"), str(script), "exec"), namespace)
            result.update(status="passed", result=namespace.get("RESULT", {}))
        except Exception as error:
            result.update(status="failed", error=str(error))
            unreal.log_warning("MS_EDITOR_RECIPE_FAILED=" + traceback.format_exc())
        finally:
            self.busy = False
        (self.local / "editor-result.json").write_text(json.dumps(result, indent=2, default=str), encoding="utf-8")
        unreal.log("MS_EDITOR_RECIPE=" + json.dumps(result, default=str))
        self.handle = unreal.register_slate_post_tick_callback(self.tick)


try:
    unreal.unregister_slate_post_tick_callback(getattr(MS_RECIPE_QUEUE, "handle", MS_RECIPE_QUEUE_HANDLE))
except (NameError, TypeError):
    pass
MS_RECIPE_QUEUE = RecipeQueue()
MS_RECIPE_QUEUE.handle = unreal.register_slate_post_tick_callback(MS_RECIPE_QUEUE.tick)
MS_RECIPE_QUEUE_HANDLE = MS_RECIPE_QUEUE.handle
unreal.log("MS_EDITOR_RECIPE_QUEUE_READY")
