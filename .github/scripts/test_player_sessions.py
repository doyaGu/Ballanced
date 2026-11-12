"""Exercise Player tools against composition-relative game save writes."""

import contextlib
import importlib.util
import io
from pathlib import Path
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch


PLAYER_TOOLS = Path(__file__).resolve().parents[2] / "tests" / "player"


def load_tool(name):
    spec = importlib.util.spec_from_file_location(name, PLAYER_TOOLS / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    with patch.object(sys, "path", [str(PLAYER_TOOLS), *sys.path]):
        spec.loader.exec_module(module)
    return module


class PlayerSessionTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name).resolve()
        self.stage = self.root / "Game & [installation]"
        self.binaries = self.root / "binaries"
        self.output = self.root / "acceptance"
        self.binaries.mkdir()
        (self.stage / "Bin").mkdir(parents=True)
        (self.stage / "Bin/Player.ini").write_text("[Graphics]\nWidth=640\n")
        (self.stage / "base.cmo").write_bytes(b"game composition")
        (self.stage / "Database.tdb").write_bytes(b"original save")
        for directory in ("3D Entities", "Sounds", "Sounds_low", "Textures"):
            (self.stage / directory).mkdir()
            (self.stage / directory / "asset").write_bytes(b"original asset")
        self.player = self.binaries / "Player.exe"
        self.player.write_bytes(b"test executable")
        for name in ("CK2.dll", "VxMath.dll", "SDL3.dll", "CK2_3D.dll", "CKSdlGpuRasterizer.dll"):
            (self.binaries / name).write_bytes(b"compiled runtime")
        for category in ("Managers", "BuildingBlocks", "Plugins"):
            (self.stage / category).mkdir()
            name = category + ".dll"
            (self.stage / category / name).write_bytes(b"installed module")
            (self.binaries / name).write_bytes(b"compiled module")

    def emulate_game_save(self, command):
        # GamePlayer sets Gravity from base.cmo's directory. Game scripts save
        # beside that composition, regardless of the separate --data-path.
        composition = Path(next(arg.split("=", 1)[1] for arg in command if arg.startswith("--cmo=")))
        self.assertEqual(composition.read_bytes(), b"game composition")
        self.assertEqual((composition.parent / "3D Entities/asset").read_bytes(), b"original asset")
        (composition.parent / "Database.tdb").write_bytes(b"updated private save")
        self.session = composition.parent

    def assert_isolated(self):
        self.assertEqual((self.stage / "Database.tdb").read_bytes(), b"original save")
        self.assertNotEqual(self.session, self.stage)
        self.assertEqual((self.session / "Database.tdb").read_bytes(), b"updated private save")
        self.assertFalse((self.session / "base.cmo").samefile(self.stage / "base.cmo"))
        self.assertEqual((self.session / "Player.ini").read_text(), "[Graphics]\nWidth=640\n")

    def test_manual_session_writes_only_its_private_save(self):
        manual = load_tool("player_ff_jit_manual")
        arguments = SimpleNamespace(player=self.player, binaries=self.binaries,
                                    stage=self.stage, out=self.output)
        with contextlib.redirect_stdout(io.StringIO()):
            manual.prepare(arguments)
        subprocess.run([sys.executable, "-B", str(self.output / "launch.py"), "--help"],
                       cwd=self.output, check=True, capture_output=True, text=True)

        def player_run(command, **kwargs):
            self.emulate_game_save(command)
            kwargs["stdout"].write("FFJIT_STATS ready=1 compile_failed=0 vertex_failed=0 pipeline_failed=0\n")
            return SimpleNamespace(returncode=0)

        real_run = subprocess.run
        def run(command, **kwargs):
            if command[0] == str(self.output / "runtime/Bin/Player.exe"):
                return player_run(command, **kwargs)
            return real_run(command, **kwargs)

        with patch.object(manual, "__file__", str(self.output / "launch.py")), \
             patch.object(manual.subprocess, "run", side_effect=run), \
             contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(manual.play(SimpleNamespace(driver="vulkan", mode="on")), 0)
        self.assert_isolated()

    def test_authored_smoke_writes_only_its_private_save(self):
        smoke = load_tool("player_ff_jit_smoke")
        class PlayerStopped(Exception):
            pass

        real_run = subprocess.run
        def run(command, **kwargs):
            if command[0] == str(self.output / "runtime/Bin/Player.exe"):
                self.emulate_game_save(command)
                raise PlayerStopped()
            return real_run(command, **kwargs)

        arguments = [str(PLAYER_TOOLS / "player_ff_jit_smoke.py"),
                     "--player", str(self.player), "--binaries", str(self.binaries),
                     "--stage", str(self.stage), "--out", str(self.output),
                     "--drivers", "vulkan", "--modes", "off", "--scenes", "menu"]
        with patch.object(sys, "argv", arguments), patch.object(smoke.subprocess, "run", side_effect=run):
            with self.assertRaises(PlayerStopped):
                smoke.main()
        self.assert_isolated()


if __name__ == "__main__":
    unittest.main()
