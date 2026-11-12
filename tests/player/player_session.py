"""Create a composition-relative save directory for a Player test session."""

import json
import os
import shutil
import subprocess


ASSET_DIRECTORIES = ("3D Entities", "Sounds", "Sounds_low", "Textures")


def prepare_session(stage, session):
    session.mkdir(parents=True, exist_ok=True)
    for source, name in ((stage / "base.cmo", "base.cmo"),
                         (stage / "Database.tdb", "Database.tdb"),
                         (stage / "Bin/Player.ini", "Player.ini")):
        shutil.copy2(source, session / name)
    # GamePlayer sets Gravity from the composition's directory. Copying only
    # Database.tdb and setting --data-path leaves game-script saves in stage.
    links = [{"path": str(session / name), "target": str((stage / name).resolve())}
             for name in ASSET_DIRECTORIES if (stage / name).is_dir()]
    if os.name == "nt" and links:
        # Directory junctions work without the symbolic-link privilege. Pass
        # paths as data so spaces and shell metacharacters remain literal.
        env = os.environ.copy()
        env["BALLANCED_SESSION_LINKS"] = json.dumps(links)
        subprocess.run([
            "powershell.exe", "-NoProfile", "-NonInteractive", "-Command",
            "$ErrorActionPreference = 'Stop'; "
            "$links = ConvertFrom-Json $env:BALLANCED_SESSION_LINKS; "
            "foreach ($link in $links) { New-Item -ItemType Junction "
            "-Path $link.path -Target $link.target | Out-Null }",
        ], env=env, check=True)
    else:
        for link in links:
            (session / os.path.basename(link["path"])).symlink_to(link["target"], target_is_directory=True)
    return session / "base.cmo"
