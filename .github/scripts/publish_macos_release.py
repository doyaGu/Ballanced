"""Replace macOS release assets only after all four notarized images pass."""

import argparse
import hashlib
import json
from pathlib import Path
import tempfile
from urllib.parse import quote

from macos_release import run


def release_files(directory, revision):
    expected = {f"ballance-sdl-macos-{arch}-{mode}.dmg"
                for arch in ("x64", "arm64") for mode in ("runtime", "static")}
    images = list(directory.rglob("*.dmg"))
    if len(images) != 4 or {image.name for image in images} != expected:
        raise RuntimeError("All four macOS release images are required before publishing")
    for image in images:
        manifest = json.loads(image.with_suffix(".json").read_text())
        if (manifest.get("name") != image.name or manifest.get("source_sha") != revision
                or not manifest.get("notarization_id")
                or manifest.get("sha256") != hashlib.sha256(image.read_bytes()).hexdigest()):
            raise RuntimeError(f"Release manifest mismatch: {image.name}")
    return sorted(images)


def remote_tag(repo, tag):
    run(["git", "check-ref-format", "refs/tags/" + tag])
    obj = json.loads(run(["gh", "api", f"repos/{repo}/git/ref/tags/{quote(tag, safe='')}"]).stdout)["object"]
    for _ in range(16):
        if obj["type"] == "commit":
            return obj["sha"]
        if obj["type"] != "tag":
            break
        obj = json.loads(run(["gh", "api", f"repos/{repo}/git/tags/{obj['sha']}"]).stdout)["object"]
    raise RuntimeError("Release tag does not resolve to a commit")


def publish(directory, repo, tag, revision):
    images = release_files(directory, revision)
    if remote_tag(repo, tag) != revision:
        raise RuntimeError("Release tag moved after source resolution; refusing to replace assets")
    run(["gh", "release", "view", tag, "--repo", repo])
    run(["gh", "release", "upload", tag, *images, "--repo", repo, "--clobber"])
    release = json.loads(run(["gh", "release", "view", tag, "--repo", repo, "--json", "assets,body"]).stdout)
    assets = {asset["name"]: asset for asset in release["assets"]}
    for image in images:
        asset = assets.get(image.name, {})
        digest = "sha256:" + hashlib.sha256(image.read_bytes()).hexdigest()
        if asset.get("state") != "uploaded" or asset.get("digest") != digest:
            raise RuntimeError(f"Uploaded asset verification failed: {image.name}")
    # Remove old unsigned tarballs only after every replacement is uploaded.
    body = release.get("body") or ""
    for image in images:
        old_name = image.name.removesuffix(".dmg") + ".tar.gz"
        if old_name in assets:
            run(["gh", "release", "delete-asset", tag, old_name, "--repo", repo, "--yes"])
        body = body.replace(old_name, image.name)
    with tempfile.TemporaryDirectory() as temporary:
        notes = Path(temporary) / "release.md"
        notes.write_text(body, encoding="utf-8")
        run(["gh", "release", "edit", tag, "--repo", repo, "--notes-file", notes])
    print(f"Published four notarized macOS DMGs for {tag} at {revision}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--directory", required=True, type=Path)
    parser.add_argument("--repo", required=True)
    parser.add_argument("--tag", required=True)
    parser.add_argument("--revision", required=True)
    args = parser.parse_args()
    publish(args.directory, args.repo, args.tag, args.revision)


if __name__ == "__main__":
    main()
