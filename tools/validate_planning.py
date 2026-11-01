"""Validate this handoff and optionally regenerate its human-readable backlog.

Uses only the Python standard library. Does not run gameplay tests, fetch links,
or implement a general JSON Schema validator.
"""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import json
import math
from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlsplit


ROOT = Path(__file__).resolve().parents[1]


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"Duplicate JSON key: {key}")
        result[key] = value
    return result


def reject_constant(value):
    raise ValueError(f"Nonfinite JSON constant: {value}")


def load_json(path):
    return json.loads(path.read_text(encoding="utf-8"),
                      object_pairs_hook=unique_object, parse_constant=reject_constant)


def numeric(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value)


def render_backlog(backlog, milestones):
    tasks = backlog["tasks"]
    lines = ["# Meat Shield implementation backlog", "",
             "Generated from `planning/backlog.json` and `planning/milestones.json`. "
             "Edit those files, then run `python tools/validate_planning.py --write-backlog`.", "",
             "All tasks are planned, unassigned baseline work. Estimates are person-days, "
             "not elapsed days. Low and high values are scenarios, not confidence bounds. "
             "Dependencies indicate deliverable prerequisites; the producer must schedule "
             "named capacity and incremental integration before committing dates.", "",
             "See [requirements](../../planning/requirements.json), "
             "[production plan](production-plan.md), and [test plan](../quality/test-plan.md).", "",
             "## Capacity and effort", "",
             "| Role | Baseline capacity | Likely named work | Available for iteration |",
             "| --- | ---: | ---: | ---: |"]
    for role in milestones["roles"]:
        capacity = sum(m["capacity_days"][role] for m in milestones["milestones"])
        demand = sum(t["estimate_days"]["likely"] for t in tasks if t["role"] == role)
        lines.append(f"| {role} | {capacity:g} | {demand:g} | {capacity - demand:g} |")
    likely = sum(t["estimate_days"]["likely"] for t in tasks)
    high = sum(t["estimate_days"]["high"] for t in tasks)
    lines.extend(["", f"Named work: **{likely:g} likely days**, **{high:g} days at summed upper "
                  "bounds**. QA and audio have no unassigned baseline capacity; book extra "
                  "specialist time if their upper scenarios materialize.", ""])
    for milestone in milestones["milestones"]:
        mid = milestone["id"]
        lines.extend([f"## {mid} {milestone['title']}", "",
                      f"Relative weeks {milestone['start_week']}–{milestone['end_week']}.", ""])
        for gate in milestone["gates"]:
            lines.append(f"- {gate}")
        lines.extend(["", "| ID | Task | Role | Likely days | Dependencies |",
                      "| --- | --- | --- | ---: | --- |"])
        selected = [t for t in tasks if t["milestone"] == mid]
        for task in selected:
            deps = ", ".join(task["dependencies"]) or "None"
            lines.append(f"| {task['id']} | {task['title']} | {task['role']} | "
                         f"{task['estimate_days']['likely']:g} | {deps} |")
        lines.append("")
        for task in selected:
            estimate = task["estimate_days"]
            lines.extend([f"### {task['id']} {task['title']}", "", task["deliverable"] + ".", "",
                          f"Requirements: {', '.join(task['requirements'])}. "
                          f"Estimate: {estimate['low']:g} / {estimate['likely']:g} / "
                          f"{estimate['high']:g} days, low / likely / high.", ""])
            for condition in task["acceptance"]:
                lines.append(f"- [ ] {condition}")
            lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def validate(root=ROOT, write_backlog=False):
    errors = []

    def check(condition, message):
        if not condition:
            errors.append(message)

    names = ["requirements.json", "backlog.json", "milestones.json", "assets.json",
             "tuning.example.json", "save.example.json", "telemetry.schema.json"]
    try:
        data = {name: load_json(root / "planning" / name) for name in names}
        reqs = data["requirements.json"]["requirements"]
        backlog = data["backlog.json"]
        tasks = backlog["tasks"]
        milestone_data = data["milestones.json"]
        milestones = milestone_data["milestones"]
        assets = data["assets.json"]["assets"]
        tuning = data["tuning.example.json"]
        save = data["save.example.json"]
        telemetry = data["telemetry.schema.json"]
    except (OSError, ValueError, KeyError, TypeError) as exc:
        return [f"Cannot read planning data: {exc}"], {}

    def index(records, label, pattern):
        check(isinstance(records, list) and bool(records), f"{label}: expected nonempty array")
        ids = [record["id"] for record in records]
        check(len(ids) == len(set(ids)), f"{label}: duplicate IDs")
        for item in ids:
            check(bool(re.fullmatch(pattern, item)), f"{label}: invalid ID {item}")
        return {record["id"]: record for record in records}

    try:
        req_index = index(reqs, "Requirements", r"REQ-\d{3}")
        task_index = index(tasks, "Tasks", r"MS-\d{3}")
        milestone_index = index(milestones, "Milestones", r"M\d+")
        index(assets, "Assets", r"AST-\d{3}")
        order = {milestone["id"]: i for i, milestone in enumerate(milestones)}
        roles = milestone_data["roles"]
        check(len(roles) == len(set(roles)), "Duplicate role names")
        next_week = 1
        for milestone in milestones:
            check(milestone["start_week"] == next_week, f"{milestone['id']}: noncontiguous weeks")
            check(milestone["end_week"] >= milestone["start_week"], "Invalid milestone duration")
            next_week = milestone["end_week"] + 1
            check(bool(milestone["gates"]), f"{milestone['id']}: missing gate conditions")
            check(set(milestone["capacity_days"]) == set(roles), "Capacity role mismatch")
            for capacity in milestone["capacity_days"].values():
                check(numeric(capacity) and capacity >= 0, "Invalid role capacity")
        check(next_week - 1 == milestone_data["baseline_weeks"], "Baseline week total mismatch")
        check(milestone_data["contingency_weeks"] > 0, "Missing contingency duration")
        extension = milestone_data["contingency_capacity_days"]
        check(set(extension) == set(roles), "Contingency role mismatch")
        check(all(numeric(v) and v >= 0 for v in extension.values()), "Invalid contingency capacity")

        test_text = (root / "docs/quality/test-plan.md").read_text(encoding="utf-8")
        test_map = {}
        for line in test_text.splitlines():
            match = re.match(r"\| (TC-\d{3}) \| ([^|]+) \|", line)
            if match:
                check(match[1] not in test_map, f"Duplicate case {match[1]}")
                test_map[match[1]] = set(re.findall(r"REQ-\d{3}", match[2]))
        for req in reqs:
            rid = req["id"]
            check(req["provenance"] in {"source", "analysis", "proposed"}, f"{rid}: invalid provenance")
            check(req["gate"] in order, f"{rid}: unknown gate")
            check((root / req["document"]).is_file(), f"{rid}: missing source document")
            check(bool(req["title"].strip()) and bool(req["acceptance"].strip()), f"{rid}: empty requirement")
            check(bool(req["tests"]), f"{rid}: no tests")
            for case in req["tests"]:
                check(rid in test_map.get(case, set()), f"{rid}: {case} missing or does not map back")
        for case, mapped in test_map.items():
            for rid in mapped:
                check(rid in req_index and case in req_index[rid]["tests"], f"{case}: inconsistent {rid} mapping")

        coverage = defaultdict(list)
        demand = Counter()
        for task in tasks:
            tid = task["id"]
            check(task["role"] in roles, f"{tid}: unknown role")
            check(task["milestone"] in order, f"{tid}: unknown milestone")
            check(task["status"] in {"planned", "active", "done", "blocked"}, f"{tid}: invalid status")
            check(task["priority"] in {"P0", "P1", "P2"}, f"{tid}: invalid priority")
            check(all(task[key].strip() for key in ["title", "epic", "deliverable"]), f"{tid}: missing text")
            check(len(task["acceptance"]) >= 2 and all(c.strip() for c in task["acceptance"]), f"{tid}: missing acceptance")
            estimate = task["estimate_days"]
            values = [estimate[key] for key in ("low", "likely", "high")]
            check(all(numeric(v) and v >= 0 for v in values), f"{tid}: invalid effort")
            check(values[0] <= values[1] <= values[2], f"{tid}: unordered effort range")
            demand[(task["milestone"], task["role"])] += estimate["likely"]
            check(len(task["dependencies"]) == len(set(task["dependencies"])), f"{tid}: duplicate dependency")
            for dep in task["dependencies"]:
                check(dep in task_index and dep != tid, f"{tid}: invalid dependency {dep}")
                if dep in task_index and task["milestone"] in order:
                    check(order.get(task_index[dep]["milestone"], 999) <= order[task["milestone"]],
                          f"{tid}: depends on later gate {dep}")
            check(bool(task["requirements"]), f"{tid}: no requirements")
            for rid in task["requirements"]:
                check(rid in req_index, f"{tid}: unknown requirement {rid}")
                coverage[rid].append(task)

        visiting, visited = set(), set()

        def visit(tid):
            if tid in visiting:
                errors.append(f"Dependency cycle at {tid}")
                return
            if tid in visited:
                return
            visiting.add(tid)
            for dep in task_index[tid]["dependencies"]:
                if dep in task_index:
                    visit(dep)
            visiting.remove(tid)
            visited.add(tid)

        for tid in task_index:
            visit(tid)
        for req in reqs:
            check(any(order.get(t["milestone"], 999) <= order.get(req["gate"], -1)
                      for t in coverage[req["id"]]), f"{req['id']}: no work by target gate")
        for milestone in milestones:
            for role in roles:
                used = demand[(milestone["id"], role)]
                check(used <= milestone["capacity_days"][role],
                      f"{milestone['id']} {role}: likely effort {used:g} exceeds capacity")
        # The prose resource table is intentionally checked against its data.
        production_text = (root / "docs/production/production-plan.md").read_text(encoding="utf-8")
        capacity_total = sum(sum(m["capacity_days"].values()) for m in milestones)
        likely_total = sum(t["estimate_days"]["likely"] for t in tasks)
        total_row = (f"| Total | Capacity model | {capacity_total:g} | {likely_total:g} | "
                     f"{capacity_total - likely_total:g} |")
        check(total_row in production_text, "Production resource table total is stale")
        for role in roles:
            capacity = sum(m["capacity_days"][role] for m in milestones)
            used = sum(t["estimate_days"]["likely"] for t in tasks if t["role"] == role)
            pattern = (rf"\| {re.escape(role)} \| [^|]+ \| {capacity:g} \| {used:g} \| "
                       rf"{capacity - used:g} \|")
            check(bool(re.search(pattern, production_text)), f"Production resource table stale for {role}")
        for asset in assets:
            check(asset["owner_role"] in roles, f"{asset['id']}: unknown asset owner")
            check(asset["milestone"] in order, f"{asset['id']}: unknown asset gate")
            check(numeric(asset["quantity"]) and asset["quantity"] > 0, "Invalid asset quantity")
            check(asset["task"] in task_index, f"{asset['id']}: unknown task")
            if asset["task"] in task_index:
                task = task_index[asset["task"]]
                check(order[task["milestone"]] <= order[asset["milestone"]], f"{asset['id']}: task after asset gate")
                check(task["role"] == asset["owner_role"], f"{asset['id']}: task owner mismatch")
            check(bool(asset["acceptance"].strip()), f"{asset['id']}: missing acceptance")

        construction = tuning["construction"]
        check(construction["max_tiles_per_sector"] * construction["max_primitives_per_tile"] ==
              construction["max_primitives_per_sector"], "Construction capacity relationship mismatch")
        check(construction["max_primitives_per_body"] <= construction["max_primitives_per_tile"], "Body exceeds tile quota")
        check(tuning["weapon"]["ammo_persists_through_death"] is True, "Ammo persistence mismatch")
        check(tuning["weapon"]["reload_progress_persists_through_death"] is True, "Reload persistence mismatch")
        check(tuning["threat"]["suppression_refresh_allowed"] is False, "Suppression refresh mismatch")
        check(save["rules_version"] == tuning["rules_version"], "Example rules mismatch")
        check(save["classification"] in {"uninterrupted", "resumed", "assisted"}, "Invalid run class")
        check(save["active_time_s"] >= 0 and save["deployments"] >= 0, "Negative run values")
        check(0 <= save["weapon"]["magazine"] <= tuning["weapon"]["magazine_capacity"], "Magazine out of range")
        check(0 <= save["weapon"]["reserve"] <= tuning["weapon"]["starting_reserve"], "Reserve out of range")
        bodies = save["construction"]
        body_ids = [body["id"] for body in bodies]
        check(len(body_ids) == len(set(body_ids)), "Duplicate body IDs")
        known_bodies, occupancy, shape_ids = set(), Counter(), set()
        for body in bodies:
            check(body["pose_id"] in {"prone-v1", "compact-wall-v1"}, "Unknown example pose")
            check(len(body["position_m"]) == 3 and all(numeric(v) for v in body["position_m"]), "Invalid body position")
            check(numeric(body["yaw_deg"]), "Invalid body yaw")
            check(0 < len(body["shapes"]) <= construction["max_primitives_per_body"], "Body shape budget exceeded")
            check(bool(body["supports"]), "Unsupported example body")
            for support in body["supports"]:
                check(support.startswith("world:") or support in known_bodies, "Forward missing or cyclic body support")
            known_bodies.add(body["id"])
            for shape in body["shapes"]:
                check(shape["id"] not in shape_ids, "Duplicate shape ID")
                shape_ids.add(shape["id"])
                check(shape["kind"] == "box", "Unsupported example shape")
                check(bool(shape["tile_ids"]) and len(shape["tile_ids"]) == len(set(shape["tile_ids"])), "Invalid shape tiles")
                for key in ["center_m", "half_extents_m"]:
                    check(len(shape[key]) == 3 and all(numeric(v) for v in shape[key]), "Invalid shape vector")
                check(all(v > 0 for v in shape["half_extents_m"]), "Nonpositive shape extent")
                occupancy.update(shape["tile_ids"])
        check(dict(occupancy) == save["tile_occupancy"], "Saved tile occupancy mismatch")
        check(all(count <= construction["max_primitives_per_tile"] for count in occupancy.values()), "Example tile over capacity")
        for threat in save["threats"]:
            check(threat["state"] in {"Idle", "Acquire", "Telegraph", "Burst", "Recover", "Suppressed", "Disabled"}, "Unknown threat state")
            check(numeric(threat["remaining_s"]) and threat["remaining_s"] >= 0, "Invalid threat timer")

        # Check the schema's event coverage and example, not arbitrary JSON Schema.
        branches = {b["properties"]["event_name"]["const"]: b for b in telemetry["oneOf"]}
        check(set(branches) == set(telemetry["properties"]["event_name"]["enum"]), "Event schema branch mismatch")
        for event in telemetry["examples"]:
            check(set(event) == set(telemetry["required"]), "Event example envelope mismatch")
            check(event["event_name"] in branches, "Unknown event example")
            branch = branches[event["event_name"]]["properties"]["payload"]
            check(set(event["payload"]) == set(branch["required"]), "Event payload fields mismatch")
            check(event["schema_version"] == 1 and event["rules_version"] == tuning["rules_version"], "Event version mismatch")
            check(numeric(event["active_time_s"]) and event["active_time_s"] >= 0, "Invalid event timestamp")
            for key, value in event["payload"].items():
                field = branch["properties"][key]
                if field.get("type") == "number":
                    check(numeric(value) and value >= field.get("minimum", -math.inf), f"Invalid event number {key}")
                elif field.get("type") == "string":
                    check(isinstance(value, str) and bool(value), f"Invalid event string {key}")

        generated = render_backlog(backlog, milestone_data)
        target = root / "docs/production/backlog.md"
        if write_backlog and not errors:
            target.write_text(generated, encoding="utf-8", newline="\n")
        check(target.exists() and target.read_text(encoding="utf-8") == generated,
              "Generated backlog is missing or stale; run with --write-backlog")
    except (KeyError, TypeError, ValueError, OSError, AttributeError) as exc:
        errors.append(f"Invalid planning structure: {exc}")

    markdown = [path for path in root.rglob("*.md") if ".git" not in path.relative_to(root).parts]
    for path in markdown:
        content = path.read_text(encoding="utf-8")
        for match in re.finditer(r"!?\[[^\]]*\]\(([^\s)]+)(?:\s+\"[^\"]*\")?\)", content):
            link = match[1].strip("<>")
            parsed = urlsplit(link)
            if parsed.scheme or not parsed.path:
                continue
            resolved = (path.parent / unquote(parsed.path)).resolve()
            check(resolved.is_relative_to(root.resolve()), f"{path.relative_to(root)}: link escapes repository {link}")
            check(resolved.exists(), f"{path.relative_to(root)}: broken link {link}")

    summary = {"requirements": len(reqs), "tasks": len(tasks), "milestones": len(milestones),
               "assets": len(assets), "markdown_files": len(markdown),
               "likely_days": sum(t["estimate_days"]["likely"] for t in tasks),
               "baseline_capacity_days": sum(sum(m["capacity_days"].values()) for m in milestones)}
    return errors, summary


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write-backlog", action="store_true", help="Regenerate the backlog from JSON")
    args = parser.parse_args()
    errors, summary = validate(write_backlog=args.write_backlog)
    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1
    print("Planning validation passed: " + json.dumps(summary, sort_keys=True))
    print("Gameplay, engine builds, external links, and general JSON Schema validation were not run.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
