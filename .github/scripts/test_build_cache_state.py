import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from build_cache_state import STATE_FILE, restore, save


class BuildCacheStateTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name).resolve()
        self.build = self.root / "build" / "test"
        subprocess.run(["git", "init", "-q", str(self.root)], check=True)
        self.source = self.root / "source.cpp"
        self.source.write_text("int value = 1;\n")
        subprocess.run(["git", "add", "source.cpp"], cwd=self.root, check=True)
        self.build.mkdir(parents=True)

    def test_unchanged_sources_keep_their_cached_timestamps(self):
        original = self.source.stat().st_mtime_ns
        save(self.root, self.build)
        os.utime(self.source, ns=(original + 10_000_000_000,) * 2)
        restore(self.root, self.build)
        self.assertEqual(self.source.stat().st_mtime_ns, original)

    def test_changed_and_added_sources_are_newer_than_cached_outputs(self):
        save(self.root, self.build)
        saved = json.loads((self.build / STATE_FILE).read_text())["saved_at_ns"]
        self.source.write_text("int value = 2;\n")
        os.utime(self.source, ns=(1, 1))
        added = self.root / "new.h"
        added.write_text("new input\n")
        subprocess.run(["git", "add", "new.h"], cwd=self.root, check=True)
        restore(self.root, self.build)
        self.assertGreater(self.source.stat().st_mtime_ns, saved)
        self.assertGreater(added.stat().st_mtime_ns, saved)

    def test_reports_and_old_package_layout_are_not_reused(self):
        save(self.root, self.build)
        for name in ("stage", "stage-install-snapshot", "Testing"):
            (self.build / name).mkdir()
            (self.build / name / "stale").touch()
        (self.build / "ctest-results.xml").touch()
        binary = self.build / "program.exe"
        binary.write_bytes(b"compiled artifact")
        restore(self.root, self.build)
        self.assertTrue(binary.exists())
        for name in ("stage", "stage-install-snapshot", "Testing", "ctest-results.xml"):
            self.assertFalse((self.build / name).exists())

    def test_different_workspace_invalidates_absolute_build_paths(self):
        save(self.root, self.build)
        state = json.loads((self.build / STATE_FILE).read_text())
        state["workspace"] = "/another/checkout"
        (self.build / STATE_FILE).write_text(json.dumps(state))
        restore(self.root, self.build)
        self.assertFalse(self.build.exists())

    def test_partial_or_corrupt_metadata_falls_back_to_a_clean_build(self):
        (self.build / "CMakeCache.txt").touch()
        restore(self.root, self.build)
        self.assertFalse(self.build.exists())
        self.build.mkdir()
        (self.build / STATE_FILE).write_text("incomplete json {")
        restore(self.root, self.build)
        self.assertFalse(self.build.exists())

    def test_restored_sources_do_not_hide_real_ninja_rebuilds(self):
        self.source.write_text('#include "value.h"\nint main() { return VALUE; }\n')
        header = self.root / "value.h"
        header.write_text("#define VALUE 0\n")
        (self.root / "CMakeLists.txt").write_text(
            "cmake_minimum_required(VERSION 3.16)\nproject(CacheTest CXX)\n"
            "add_executable(program source.cpp)\n"
        )
        subprocess.run(["git", "add", "."], cwd=self.root, check=True)
        subprocess.run(["cmake", "-S", str(self.root), "-B", str(self.build), "-G", "Ninja"],
                       check=True, stdout=subprocess.DEVNULL)
        command = ["cmake", "--build", str(self.build)]
        subprocess.run(command, check=True, stdout=subprocess.DEVNULL)
        save(self.root, self.build)
        binary = self.build / ("program.exe" if os.name == "nt" else "program")
        original = binary.stat().st_mtime_ns
        os.utime(self.source, None)
        os.utime(header, None)
        restore(self.root, self.build)
        subprocess.run(command, check=True, stdout=subprocess.DEVNULL)
        self.assertEqual(binary.stat().st_mtime_ns, original)
        header.write_text("#define VALUE 7\n")
        os.utime(header, ns=(1, 1))
        restore(self.root, self.build)
        subprocess.run(command, check=True, stdout=subprocess.DEVNULL)
        self.assertEqual(subprocess.run([str(binary)]).returncode, 7)


if __name__ == "__main__":
    unittest.main()
