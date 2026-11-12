"""Prepare or launch a manual Player JIT session with private saves and logs.

No game assets are copied into the package. Gameplay and menu navigation are
entirely manual. Run `prepare --help` to assemble a package, then launch its
launch.py with `play --driver vulkan` (or direct3d12).
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def prepare(args):
    player, binaries, stage, out = (p.resolve() for p in
                                   (args.player, args.binaries, args.stage, args.out))
    if out.exists() and any(out.iterdir()):
        raise ValueError("output must be empty; choose a new package directory")
    required = [stage / name for name in ("base.cmo", "Database.tdb", "Bin/Player.ini")]
    copies = [(player, Path("Bin/Player.exe"))]
    for name in ("CK2.dll", "VxMath.dll", "SDL3.dll"):
        copies.append((binaries / name, Path("Bin") / name))
    for name in ("CK2_3D.dll", "CKSdlGpuRasterizer.dll"):
        copies.append((binaries / name, Path("RenderEngines") / name))
    for category in ("Managers", "BuildingBlocks", "Plugins"):
        plugins = sorted((stage / category).glob("*.dll"))
        if not plugins:
            raise ValueError(f"empty staged category: {category}")
        copies += [(binaries / p.name, Path(category) / p.name) for p in plugins]
    # Preserve the tested renderer configuration when one is supplied.
    if (binaries / "CK2_3D.ini").is_file():
        copies.append((binaries / "CK2_3D.ini", Path("RenderEngines/CK2_3D.ini")))
    missing = [str(p) for p in required + [source for source, _ in copies] if not p.is_file()]
    if missing:
        raise ValueError("missing required files: " + ", ".join(missing))
    out.mkdir(parents=True, exist_ok=True)
    identities = []
    for source, relative in copies:
        target = out / "runtime" / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        identities.append(dict(source=str(source), runtime=relative.as_posix(), sha256=digest(target)))
    (out / "binaries.json").write_text(json.dumps(identities, indent=2), encoding="utf-8")
    (out / "settings.json").write_text(json.dumps(dict(stage=str(stage)), indent=2), encoding="utf-8")
    shutil.copy2(__file__, out / "launch.py")
    for driver in ("direct3d12", "vulkan"):
        (out / f"play-{driver}.cmd").write_bytes(
            f'@echo off\r\npython "%~dp0launch.py" play --driver {driver}\r\npause\r\n'.encode("ascii"))
    (out / "README.txt").write_text(
        "Manual CKShaderJIT acceptance package\n\n"
        "Double-click play-vulkan.cmd or play-direct3d12.cmd.\n"
        "Alternatively: python launch.py play --driver vulkan --mode on\n"
        "Modes: on (full JIT), fragment, off. Default: full JIT.\n"
        "Navigate and play normally; exit through the game to finish the log.\n"
        "Each session has separate Player.ini, Database.tdb and logs in sessions/.\n"
        "No automated input or level selection is performed. Game assets remain\n"
        "in the legal installation named by settings.json.\n"
        "The JIT manifest is disabled for repeatable cold sessions; the GPU driver's\n"
        "own cache is not cleared. Runtime hashes are checked before each launch.\n"
        "A clean log is not evidence of visual correctness or level completion.\n",
        encoding="utf-8")
    print(f"Prepared {len(identities)} runtime files: {out}")


def play(args):
    root = Path(__file__).resolve().parent
    runtime = root / "runtime"
    stage = Path(json.loads((root / "settings.json").read_text(encoding="utf-8"))["stage"])
    identities = json.loads((root / "binaries.json").read_text(encoding="utf-8"))
    for entry in identities:
        path = runtime / entry["runtime"]
        if not path.is_file() or digest(path) != entry["sha256"]:
            raise ValueError(f"runtime identity changed: {entry['runtime']}")
    session = root / "sessions" / (datetime.now(timezone.utc).strftime("%Y%m%d-%H%M%S-%fZ") +
                                    f"-{args.driver}-{args.mode}")
    session.mkdir(parents=True)
    shutil.copy2(stage / "Bin/Player.ini", session / "Player.ini")
    shutil.copy2(stage / "Database.tdb", session / "Database.tdb")
    env = os.environ.copy()
    env.update(CKRE_SDL_GPU_DRIVER=args.driver, CKRE_SDL_GPU_FF_JIT_CACHE="0",
               CKRE_SDL_GPU_FF_JIT="0" if args.mode == "off" else "1",
               CKRE_SDL_GPU_FF_VERTEX_JIT="0" if args.mode == "fragment" else "1",
               CKRE_SDL_GPU_FF_JIT_STATS="1")
    settings = runtime / "RenderEngines/CK2_3D.ini"
    if settings.is_file():
        env["CKRE_SETTINGS_FILE"] = str(settings)
    command = [str(runtime / "Bin/Player.exe"), f"--root-path={stage}",
               f'--render-engine-path={runtime / "RenderEngines"}',
               f'--manager-path={runtime / "Managers"}',
               f'--building-block-path={runtime / "BuildingBlocks"}',
               f'--plugin-path={runtime / "Plugins"}', f'--cmo={stage / "base.cmo"}',
               f'--config={session / "Player.ini"}', f'--log={session / "Player.log"}',
               f"--data-path={session}", "--rasterizer=sdlgpu", "--skip-opening", "--verbose"]
    print(f"Manual session: {args.driver}, JIT {args.mode}\nLogs: {session}", flush=True)
    with (session / "stdout.log").open("w", encoding="utf-8") as log:
        code = subprocess.run(command, cwd=runtime / "Bin", env=env, stdout=log, stderr=subprocess.STDOUT).returncode
    text = (session / "stdout.log").read_text(encoding="utf-8", errors="replace")
    lines = re.findall(r"FFJIT_STATS ([^\r\n]+)", text)
    stats = {k: int(v) for k, v in re.findall(r"(\w+)=(\d+)", lines[-1])} if lines else {}
    issues = []
    if code:
        issues.append(f"Player exited {code}")
    if not stats:
        issues.append("missing JIT statistics")
    if any(stats.get(k, 0) for k in ("compile_failed", "vertex_failed", "pipeline_failed")):
        issues.append("shader or pipeline creation failure")
    if args.mode != "off" and not stats.get("ready", 0):
        issues.append("no ready JIT draw observed")
    if args.mode == "off" and (stats.get("selected", 0) or stats.get("compile_queued", 0)):
        issues.append("disabled run used JIT")
    if args.mode != "on" and any(stats.get(k, 0) for k in ("positiont", "unlit", "lit", "tween", "blend", "clip", "pad")):
        issues.append("disabled vertex JIT was used")
    result = dict(driver=args.driver, mode=args.mode, exit=code, stats=stats, issues=issues,
                  command=command, runtimeIdentities=identities,
                  visualAcceptance="awaiting user feedback", levelCompletion="not inferred")
    (session / "session.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
    print(json.dumps(dict(exit=code, stats=stats, issues=issues), indent=2))
    return code or int(bool(issues))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    package = commands.add_parser("prepare")
    for name in ("player", "binaries", "stage", "out"):
        package.add_argument("--" + name, required=True, type=Path)
    launch = commands.add_parser("play")
    launch.add_argument("--driver", choices=("direct3d12", "vulkan"), default="vulkan")
    launch.add_argument("--mode", choices=("on", "fragment", "off"), default="on")
    args = parser.parse_args()
    try:
        return prepare(args) if args.command == "prepare" else play(args)
    except (OSError, ValueError, KeyError) as error:
        print(str(error), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
