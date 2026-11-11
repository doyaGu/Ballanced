"""Sign, notarize and verify the installed macOS runtime on a CI runner."""

import argparse
import base64
import hashlib
import json
import os
from pathlib import Path
import re
import secrets
import shlex
import shutil
import subprocess
import sys
import tempfile
import time


REQUIRED_SECRETS = (
    "MACOS_CERTIFICATE_P12_BASE64", "MACOS_CERTIFICATE_PASSWORD",
    "APPLE_ID", "APPLE_TEAM_ID", "APPLE_APP_SPECIFIC_PASSWORD",
)
MACHO_MAGIC = {
    b"\xfe\xed\xfa\xce", b"\xce\xfa\xed\xfe", b"\xfe\xed\xfa\xcf", b"\xcf\xfa\xed\xfe",
    b"\xca\xfe\xba\xbe", b"\xbe\xba\xfe\xca", b"\xca\xfe\xba\xbf", b"\xbf\xba\xfe\xca",
}


def run(args, *, sensitive=False, check=True, **kwargs):
    result = subprocess.run([str(a) for a in args], capture_output=True, text=True, **kwargs)
    if check and result.returncode:
        detail = "Check the signing certificate or notarization credentials." if sensitive else result.stdout + result.stderr
        raise RuntimeError(f"{args[0]} failed ({result.returncode}): {detail}")
    return result


def check_secrets(environment):
    missing = [name for name in REQUIRED_SECRETS if not environment.get(name)]
    if missing:
        raise RuntimeError("Missing repository secrets: " + ", ".join(missing))
    if not re.fullmatch(r"[A-Z0-9]{10}", environment["APPLE_TEAM_ID"]):
        raise RuntimeError("APPLE_TEAM_ID must contain 10 uppercase letters/digits")


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2) + "\n", encoding="utf-8")


def load_state(work):
    return json.loads((work / "state.json").read_text())


def setup(work):
    check_secrets(os.environ)
    work.mkdir(mode=0o700, parents=True, exist_ok=False)
    keychain = work / "release.keychain-db"
    previous = shlex.split(run(["security", "list-keychains", "-d", "user"]).stdout)
    state = {"keychain": str(keychain), "previous_keychains": previous,
             "profile": "BallancedNotary", "team": os.environ["APPLE_TEAM_ID"]}
    write_json(work / "state.json", state)
    password = secrets.token_urlsafe(32)
    if os.environ.get("GITHUB_ACTIONS"):
        print("::add-mask::" + password, flush=True)
    run(["security", "create-keychain", "-p", password, keychain], sensitive=True)
    run(["security", "set-keychain-settings", "-lut", "21600", keychain])
    run(["security", "unlock-keychain", "-p", password, keychain], sensitive=True)
    certificate = work / "identity.p12"
    try:
        certificate.write_bytes(base64.b64decode("".join(os.environ["MACOS_CERTIFICATE_P12_BASE64"].split()), validate=True))
        run(["security", "import", certificate, "-k", keychain,
             "-P", os.environ["MACOS_CERTIFICATE_PASSWORD"], "-T", "/usr/bin/codesign"], sensitive=True)
    finally:
        certificate.unlink(missing_ok=True)
    run(["security", "set-key-partition-list", "-S", "apple-tool:,apple:,codesign:",
         "-s", "-k", password, keychain], sensitive=True)
    run(["security", "list-keychains", "-d", "user", "-s", keychain, *previous])
    identities = run(["security", "find-identity", "-v", "-p", "codesigning", keychain]).stdout
    matches = re.findall(r'([A-Fa-f0-9]{40}) "Developer ID Application: [^"\n]+ \(' + re.escape(state["team"]) + r'\)"', identities)
    if len(matches) != 1:
        raise RuntimeError("P12 must contain exactly one valid Developer ID Application identity for APPLE_TEAM_ID, including its private key")
    state["identity"] = matches[0]
    write_json(work / "state.json", state)
    run(["xcrun", "notarytool", "store-credentials", state["profile"], "--keychain", keychain,
         "--apple-id", os.environ["APPLE_ID"], "--team-id", state["team"],
         "--password", os.environ["APPLE_APP_SPECIFIC_PASSWORD"]], sensitive=True)
    print("Imported Developer ID identity and validated notarization credentials")


def macho_files(stage):
    stage = stage.resolve()
    result = []
    for path in sorted(stage.rglob("*")):
        if path.is_symlink():
            if not path.resolve().is_relative_to(stage):
                raise RuntimeError(f"Runtime symlink leaves stage: {path}")
            continue
        if path.is_file():
            with path.open("rb") as stream:
                if stream.read(4) in MACHO_MAGIC:
                    result.append(path)
    if stage / "Bin/Player" not in result:
        raise RuntimeError("Stage must contain a Mach-O Bin/Player")
    return result


def verify_signatures(stage, team):
    for binary in macho_files(stage):
        run(["codesign", "--verify", "--strict", "--all-architectures", binary])
        details = run(["codesign", "--display", "--verbose=4", binary]).stderr
        if f"TeamIdentifier={team}" not in details or "Authority=Developer ID Application:" not in details:
            raise RuntimeError(f"Missing expected Developer ID signature: {binary}")
        if "Timestamp=" not in details:
            raise RuntimeError(f"Missing secure timestamp: {binary}")
        flags = re.search(r"flags=0x([0-9a-fA-F]+)", details)
        if binary.name == "Player" and (not flags or not int(flags[1], 16) & 0x10000):
            raise RuntimeError("Player must enable Hardened Runtime")


def sign_stage(stage, work):
    state = load_state(work)
    binaries = macho_files(stage)
    # Sign libraries before the executable, after all install_name_tool edits.
    for binary in sorted(binaries, key=lambda p: p.name == "Player"):
        args = ["codesign", "--force", "--timestamp", "--sign", state["identity"],
                "--keychain", state["keychain"]]
        if binary.name == "Player":
            args += ["--options", "runtime"]
        run([*args, binary])
    verify_signatures(stage, state["team"])
    print(f"Verified Developer ID signatures on {len(binaries)} Mach-O files")


def notarize(stage, image, work, revision):
    state = load_state(work)
    verify_signatures(stage, state["team"])
    image.parent.mkdir(parents=True, exist_ok=True)
    run(["hdiutil", "create", "-ov", "-volname", "Ballanced", "-srcfolder", stage,
         "-fs", "HFS+", "-format", "UDZO", image])
    run(["codesign", "--force", "--timestamp", "--sign", state["identity"],
         "--keychain", state["keychain"], image])
    auth = ["--keychain-profile", state["profile"], "--keychain", state["keychain"]]
    submission = run(["xcrun", "notarytool", "submit", image, *auth,
                      "--wait", "--timeout", "90m", "--output-format", "json"], check=False)
    try:
        result = json.loads(submission.stdout)
    except ValueError:
        raise RuntimeError("Notarization did not return JSON: " + submission.stderr) from None
    write_json(work / "notary-result.json", result)
    if result.get("id"):
        log = run(["xcrun", "notarytool", "log", result["id"], *auth], check=False)
        if log.returncode == 0:
            (work / "notary-log.json").write_text(log.stdout)
    if submission.returncode or result.get("status") != "Accepted":
        raise RuntimeError("Apple did not accept notarization; inspect notary-result.json and notary-log.json")
    run(["xcrun", "stapler", "staple", image])
    run(["xcrun", "stapler", "validate", image])
    run(["hdiutil", "verify", image])
    write_json(image.with_suffix(".json"), {
        "name": image.name, "source_sha": revision, "team_id": state["team"],
        "notarization_id": result["id"], "sha256": hashlib.sha256(image.read_bytes()).hexdigest(),
    })


def verify_image(image, work):
    state = load_state(work)
    run(["xcrun", "stapler", "validate", image])
    # Keep quarantine in place during the distribution test.
    with tempfile.TemporaryDirectory(prefix="download-test-", dir=work) as temporary:
        temporary = Path(temporary)
        downloaded = temporary / image.name
        shutil.copy2(image, downloaded)
        quarantine = f"0083;{int(time.time()):x};BallancedCI;"
        run(["xattr", "-w", "com.apple.quarantine", quarantine, downloaded])
        run(["spctl", "--assess", "--type", "open", "--context", "context:primary-signature", "--verbose=4", downloaded])
        mounted = temporary / "mounted"
        mounted.mkdir()
        run(["hdiutil", "attach", "-readonly", "-nobrowse", "-mountpoint", mounted, downloaded])
        try:
            verify_signatures(mounted, state["team"])
            copied = temporary / "copied runtime"
            run(["ditto", mounted, copied])
            for path in [copied, *copied.rglob("*")]:
                if not path.is_symlink():
                    run(["xattr", "-w", "com.apple.quarantine", quarantine, path])
            startup = run([copied / "Bin/Player", "--config", temporary / "Player.ini",
                           "--rasterizer", "release-verification-invalid"], check=False, timeout=30)
            if startup.returncode != 2 or "Invalid rasterizer selection" not in startup.stderr:
                raise RuntimeError("Quarantined runtime failed startup: " + startup.stdout + startup.stderr)
        finally:
            run(["hdiutil", "detach", mounted])
    print("Stapled DMG passed Gatekeeper assessment and quarantined runtime startup")


def cleanup(work):
    state_file = work / "state.json"
    if not state_file.exists():
        return
    state = load_state(work)
    run(["security", "list-keychains", "-d", "user", "-s", *state["previous_keychains"]])
    keychain = work / "release.keychain-db"
    if keychain.exists():
        run(["security", "delete-keychain", keychain])
    (work / "identity.p12").unlink(missing_ok=True)


def main():
    os.umask(0o077)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("operation", choices=("setup", "sign", "notarize", "verify", "cleanup"))
    parser.add_argument("--work-dir", type=Path, required=True)
    parser.add_argument("--stage", type=Path)
    parser.add_argument("--image", type=Path)
    parser.add_argument("--revision")
    args = parser.parse_args()
    work = args.work_dir.resolve()
    if args.operation == "setup":
        setup(work)
    elif args.operation == "cleanup":
        cleanup(work)
    elif args.operation == "sign":
        sign_stage(args.stage.resolve(), work)
    elif args.operation == "notarize":
        if not args.revision or not re.fullmatch(r"[0-9a-f]{40}", args.revision):
            parser.error("--revision must be the checked-out source commit")
        notarize(args.stage.resolve(), args.image.resolve(), work, args.revision)
    else:
        verify_image(args.image.resolve(), work)


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, OSError, ValueError) as error:
        print(f"::error::{error}", file=sys.stderr)
        sys.exit(1)
