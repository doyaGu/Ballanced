"""Preserve incremental build timestamps across a fresh Git checkout."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import time


STATE_FILE = ".ci-source-state.json"


def tracked_files(workspace):
    output = subprocess.check_output(
        ["git", "ls-files", "--recurse-submodules", "-z"], cwd=workspace
    )
    for name in output.split(b"\0"):
        if name:
            relative = os.fsdecode(name)
            path = workspace / relative
            if path.is_file() and not path.is_symlink():
                yield relative, path


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def save(workspace, build):
    files = {
        name: {"hash": digest(path), "mtime_ns": path.stat().st_mtime_ns}
        for name, path in tracked_files(workspace)
    }
    state = {
        "version": 1,
        "workspace": str(workspace),
        "build": str(build),
        "saved_at_ns": time.time_ns(),
        "files": files,
    }
    build.mkdir(parents=True, exist_ok=True)
    (build / STATE_FILE).write_text(json.dumps(state), encoding="utf-8")
    print(f"Saved incremental build state for {len(files)} source files")


def restore(workspace, build):
    state_path = build / STATE_FILE
    if not state_path.exists():
        # A cache interrupted before its metadata was saved is not reusable.
        if (build / "CMakeCache.txt").exists():
            shutil.rmtree(build)
        print("No reusable incremental build state")
        return
    try:
        state = json.loads(state_path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        shutil.rmtree(build)
        print("Discarded unreadable build cache state")
        return
    if not isinstance(state, dict) or (state.get("version"), state.get("workspace"), state.get("build")) != (
        1, str(workspace), str(build)
    ) or not isinstance(state.get("files"), dict):
        shutil.rmtree(build)
        print("Discarded build cache from a different workspace")
        return
    restored = changed = 0
    newer = max(time.time_ns(), state["saved_at_ns"] + 1_000_000)
    for name, path in tracked_files(workspace):
        previous = state["files"].get(name)
        if previous and digest(path) == previous["hash"]:
            stamp = previous["mtime_ns"]
            restored += 1
        else:
            # Changed/new inputs must be newer than every cached build output,
            # even if a checkout or runner clock supplied an older timestamp.
            stamp = newer
            changed += 1
        os.utime(path, ns=(path.stat().st_atime_ns, stamp))
    # Recreate the package layout and always generate fresh test reports.
    for name in ("stage", "stage-install-snapshot", "Testing"):
        shutil.rmtree(build / name, ignore_errors=True)
    for report in build.glob("*ctest-results*.xml"):
        report.unlink()
    print(f"Restored {restored} unchanged source timestamps; invalidated {changed} inputs")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("save", "restore"))
    parser.add_argument("--workspace", type=Path, default=Path.cwd())
    parser.add_argument("--build-dir", required=True, type=Path)
    args = parser.parse_args()
    workspace = args.workspace.resolve()
    build = (workspace / args.build_dir).resolve()
    if not build.is_relative_to(workspace / "build") or build == workspace / "build":
        parser.error("build directory must be inside the workspace's build folder")
    {"save": save, "restore": restore}[args.mode](workspace, build)


if __name__ == "__main__":
    main()
