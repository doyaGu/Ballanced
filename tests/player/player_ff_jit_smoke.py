"""Run authored menu, Level 01 entry, lifecycle and route smokes.

Requires a local staged legal game installation. Copies binaries and a private
save database, not game assets. Never pass the flat build/bin directory as a
Player plugin directory: CKPluginManager parses every plugin in that directory.
"""
import argparse
import csv
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess


def validate_gameplay(folder):
    """Require transition evidence, not just a successful process exit."""
    try:
        summary = json.loads((folder / "gameplay-summary.json").read_text())
        frames = [json.loads(line) for line in (folder / "gameplay-frames.jsonl").read_text().splitlines()]
        with (folder / "gameplay.csv").open(newline="") as stream:
            trace = list(csv.DictReader(stream))
        if summary.get("completed") is not True or summary.get("cycles") != 2:
            return ["incomplete gameplay cycles"], summary
        lives = summary["initial_lives"]
        if lives < 2 or summary["soak_game_ms"] < 20000 or summary["wall_ms"] < 20000:
            return ["incomplete gameplay duration/lives evidence"], summary
        expected = {(0, "ready"), (2, "soak")}
        phases = ("falling", "respawned", "paused", "resumed", "restart-confirmation", "restarted")
        expected.update((cycle, phase) for cycle in (1, 2) for phase in phases)
        checkpoints = {(frame["cycle"], frame["phase"]): frame for frame in frames}
        if set(checkpoints) != expected or len(frames) != len(expected):
            return ["missing or duplicate lifecycle checkpoints"], summary
        issues = []
        ready = checkpoints[0, "ready"]
        for (cycle, phase), frame in checkpoints.items():
            capture = folder / f"{cycle}-{phase}.bmp"
            if frame["capture"] != capture.name or not capture.is_file() or capture.stat().st_size <= 54:
                issues.append(f"missing {cycle}/{phase} capture")
            if frame["width"] < 320 or frame["height"] < 200 or frame["colors"] < 16 or frame["brightness"] < 16:
                issues.append(f"empty {cycle}/{phase} frame")
            expected_lives = lives - 1 if phase in ("respawned", "paused", "resumed", "restart-confirmation") else lives
            if frame["lives"] != expected_lives:
                issues.append(f"unexpected lives at {cycle}/{phase}")
            distance = (frame["x"] - ready["x"]) ** 2 + (frame["z"] - ready["z"]) ** 2
            if phase == "falling":
                if distance <= 25 or frame["y"] >= ready["y"] - 8:
                    issues.append(f"no observed movement/fall in cycle {cycle}")
            elif phase in ("ready", "respawned", "resumed", "restarted", "soak"):
                if (not frame["visible"] or distance >= 4 or abs(frame["y"] - ready["y"]) >= 2 or
                        frame["objects"] < 5 or frame["triangles"] < 1000):
                    issues.append(f"incomplete playable scene at {cycle}/{phase}")
        for cycle in (1, 2):
            rows = [row for row in trace if int(row["cycle"]) == cycle]
            if not any(row["phase"] == "moving" and row["key"] == "Up" for row in rows):
                issues.append(f"missing movement input in cycle {cycle}")
            if not any(row["phase"] == "respawning" and row["visible"] == "0" for row in rows):
                issues.append(f"missing ball removal in cycle {cycle}")
        soak = [float(row["game_ms"]) for row in trace if row["phase"] == "soak"]
        if not soak or checkpoints[2, "soak"]["game_ms"] - min(soak) < 19990:
            issues.append("missing continuous post-restart observation")
        return issues, summary
    except (OSError, ValueError, KeyError, TypeError) as error:
        return [f"invalid gameplay evidence: {error}"], None


def validate_route(folder, route_path):
    try:
        points = [line.split() for line in route_path.read_text().splitlines()
                  if line.strip() and not line.lstrip().startswith("#")]
        summary = json.loads((folder / "route-summary.json").read_text())
        if (summary.get("completed") is not True or summary["waypoints"] != len(points) or
                summary["reached"] != len(points)):
            return ["incomplete keyboard route"], summary
        with (folder / "route.csv").open(newline="") as stream:
            rows = list(csv.DictReader(stream))
        frames = [json.loads(line) for line in (folder / "gameplay-frames.jsonl").read_text().splitlines()]
        if not rows or not frames or not any(int(row["key_mask"]) for row in rows):
            return ["missing route/input observations"], summary
        issues = []
        lives = [int(row["lives"]) for row in rows]
        if lives[0] < 1 or any(after < before for before, after in zip(lives, lives[1:])):
            issues.append("life lost during route")
        if any(not math.isfinite(float(row[key])) for row in rows
               for key in ("game_ms", "x", "y", "z", "reset_x", "reset_z")):
            issues.append("invalid route coordinates")
        expected = [(i, point[4]) for i, point in enumerate(points) if len(point) == 5]
        for name, count in (("Ball_Stone", "stone_events"), ("Ball_Wood", "wood_events"), ("checkpoint", "checkpoint_events")):
            if summary[count] != sum(tag == name for _, tag in expected):
                issues.append(f"missing {name} events")
        for index, tag in expected:
            captures = [f for f in frames if f["cycle"] == index and f["phase"] == tag]
            event_rows = [row for row in rows if int(row["waypoint"]) == index]
            if len(captures) != 1 or not captures[0]["visible"] or not event_rows:
                issues.append(f"missing visible event at waypoint {index}")
                continue
            frame = captures[0]
            x, z, _, radius = map(float, points[index][:4])
            if (frame["x"] - x) ** 2 + (frame["z"] - z) ** 2 > (radius + 0.1) ** 2:
                issues.append(f"event outside waypoint {index}")
            if tag.startswith("Ball_"):
                if event_rows[0]["ball"] == tag or not any(row["ball"] == tag for row in event_rows):
                    issues.append(f"no ball-form transition at waypoint {index}")
            else:
                if not any((float(row["reset_x"]) - float(rows[0]["reset_x"])) ** 2 +
                           (float(row["reset_z"]) - float(rows[0]["reset_z"])) ** 2 > 25 for row in event_rows):
                    issues.append(f"unchanged respawn transform at waypoint {index}")
        for frame in frames:
            capture = folder / f"{frame['cycle']}-{frame['phase']}.bmp"
            if not capture.is_file() or capture.stat().st_size <= 54:
                issues.append(f"missing route capture {capture.name}")
            if frame["colors"] < 16 or frame["brightness"] < 16 or frame["width"] < 320 or frame["height"] < 200:
                issues.append(f"empty route frame {capture.name}")
        return issues, summary
    except (OSError, ValueError, KeyError, TypeError) as error:
        return [f"invalid route evidence: {error}"], None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--player", required=True, type=Path)
    parser.add_argument("--binaries", required=True, type=Path)
    parser.add_argument("--stage", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--drivers", default="direct3d12,vulkan")
    parser.add_argument("--modes", default="off,fragment,on")
    parser.add_argument("--scenes", default="menu,level")
    parser.add_argument("--route", type=Path)
    args = parser.parse_args()
    modes, scenes = args.modes.split(","), args.scenes.split(",")
    if any(mode not in ("off", "fragment", "on") for mode in modes):
        parser.error("--modes expects off,fragment,on")
    if any(scene not in ("menu", "level", "gameplay", "route") for scene in scenes):
        parser.error("--scenes expects menu,level,gameplay,route")
    if "route" in scenes and (not args.route or not args.route.is_file()):
        parser.error("--scenes route requires --route FILE")
    player, binaries, stage, output = (p.resolve() for p in
                                      (args.player, args.binaries, args.stage, args.out))
    if output.exists() and any(output.iterdir()):
        parser.error("--out must be empty; choose a fresh build-directory path")
    required = [player, stage / "base.cmo", stage / "Database.tdb", stage / "Bin/Player.ini"]
    copies = [(player, Path("Bin/Player.exe"))]
    for name in ("CK2.dll", "VxMath.dll", "SDL3.dll"):
        copies.append((binaries / name, Path("Bin") / name))
    for name in ("CK2_3D.dll", "CKSdlGpuRasterizer.dll"):
        copies.append((binaries / name, Path("RenderEngines") / name))
    # Use the installed category manifest, but take every DLL from this build.
    for category in ("Managers", "BuildingBlocks", "Plugins"):
        installed = sorted((stage / category).glob("*.dll"))
        if not installed:
            parser.error(f"missing staged plugin category: {category}")
        copies += [(binaries / p.name, Path(category) / p.name) for p in installed]
    missing = [str(p) for p in required + [src for src, _ in copies] if not p.is_file()]
    if missing:
        parser.error("missing required files: " + ", ".join(missing))
    output.mkdir(parents=True, exist_ok=True)
    if args.route:
        shutil.copy2(args.route, output / "route.txt")
    runtime = output / "runtime"
    identities = []
    for source, relative in copies:
        target = runtime / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        identities.append(dict(source=str(source), runtime=str(relative),
                               sha256=hashlib.sha256(target.read_bytes()).hexdigest()))
    (output / "binaries.json").write_text(json.dumps(identities, indent=2), encoding="utf-8")
    results = []
    for driver in args.drivers.split(","):
        for mode in modes:
            for scene in scenes:
                folder = output / driver / mode / scene
                folder.mkdir(parents=True)
                shutil.copy2(stage / "Bin/Player.ini", folder / "Player.ini")
                shutil.copy2(stage / "Database.tdb", folder / "Database.tdb")
                option = {"menu": "--stage-smoke-output", "level": "--level-entry-smoke-output",
                          "gameplay": "--gameplay-smoke-output", "route": "--gameplay-smoke-output"}[scene]
                command = [str(runtime / "Bin/Player.exe"), f"--root-path={stage}",
                           f"--render-engine-path={runtime / 'RenderEngines'}",
                           f"--manager-path={runtime / 'Managers'}",
                           f"--building-block-path={runtime / 'BuildingBlocks'}",
                           f"--plugin-path={runtime / 'Plugins'}", f"--cmo={stage / 'base.cmo'}",
                           f"--config={folder / 'Player.ini'}", f"--log={folder / 'Player.log'}",
                           f"--data-path={folder}", "--rasterizer=sdlgpu", "--skip-opening",
                           f"{option}={folder}", "--verbose"]
                if scene == "route":
                    command.append(f"--route-smoke-input={output / 'route.txt'}")
                env = os.environ.copy()
                env.update(CKRE_SDL_GPU_DRIVER=driver, CKRE_SDL_GPU_FF_JIT_CACHE="0",
                           CKRE_SDL_GPU_FF_JIT="0" if mode == "off" else "1",
                           CKRE_SDL_GPU_FF_VERTEX_JIT="0" if mode == "fragment" else "1",
                           CKRE_SDL_GPU_FF_JIT_STATS="1")
                try:
                    run = subprocess.run(command, cwd=runtime / "Bin", env=env,
                                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                         text=True, errors="replace", timeout=540 if scene == "route" else 240 if scene == "gameplay" else 90)
                    text, code = run.stdout, run.returncode
                except subprocess.TimeoutExpired as error:
                    text, code = str(error), -1
                (folder / "stdout.log").write_text(text, encoding="utf-8")
                rows = re.findall(r"FFJIT_STATS ([^\r\n]+)", text)
                stats = dict((k, int(v)) for k, v in re.findall(r"(\w+)=(\d+)", rows[-1])) if rows else {}
                capture = folder / ("main-menu.bmp" if scene == "menu" else "level01-entry.bmp")
                issues = []
                if code: issues.append(f"smoke exit {code}")
                if not capture.is_file() or capture.stat().st_size <= 54: issues.append("missing capture")
                if not stats: issues.append("missing JIT execution evidence")
                if any(stats.get(k, 0) for k in ("compile_failed", "vertex_failed", "pipeline_failed")):
                    issues.append("shader or pipeline creation failed")
                if mode == "off":
                    if stats.get("selected", 0) or stats.get("compile_queued", 0): issues.append("reference used JIT")
                elif not stats.get("ready", 0): issues.append("no ready JIT pipeline was drawn")
                if mode == "on":
                    if not stats.get("positiont", 0) or not stats.get("unlit", 0):
                        issues.append("expected generated 2D and 3D vertex draws")
                elif stats.get("positiont", 0) or stats.get("unlit", 0) or stats.get("lit", 0) or stats.get("tween", 0) or stats.get("blend", 0) or stats.get("clip", 0) or stats.get("pad", 0):
                    issues.append("disabled vertex JIT was drawn")
                gameplay = None
                route = None
                if scene == "gameplay":
                    gameplay_issues, gameplay = validate_gameplay(folder)
                    issues.extend(gameplay_issues)
                if scene == "route":
                    route_issues, route = validate_route(folder, output / "route.txt")
                    issues.extend(route_issues)
                results.append(dict(driver=driver, mode=mode, scene=scene, exit=code,
                                    stats=stats, issues=issues, gameplay=gameplay, route=route, command=command))
                (output / "results.json").write_text(json.dumps(results, indent=2), encoding="utf-8")
                print(f"{driver}/{mode}/{scene}: {'FAIL ' + '; '.join(issues) if issues else 'PASS'}", flush=True)
    return int(any(r["issues"] for r in results))


if __name__ == "__main__":
    raise SystemExit(main())
