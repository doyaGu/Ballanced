import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import macos_release as signing
import publish_macos_release as publishing


class MacOSReleaseTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name).resolve()
        self.stage = self.root / "runtime"
        (self.stage / "Bin").mkdir(parents=True)
        self.player = self.stage / "Bin/Player"
        self.player.write_bytes(b"\xcf\xfa\xed\xfe" + b"fixture")
        self.revision = "a" * 40

    def packages(self):
        for arch in ("x64", "arm64"):
            for mode in ("runtime", "static"):
                image = self.root / f"ballance-sdl-macos-{arch}-{mode}.dmg"
                image.write_bytes((arch + mode).encode())
                image.with_suffix(".json").write_text(json.dumps({
                    "name": image.name, "source_sha": self.revision,
                    "notarization_id": "accepted-submission",
                    "sha256": hashlib.sha256(image.read_bytes()).hexdigest(),
                }))

    def test_missing_secrets_report_names_without_values(self):
        with self.assertRaises(RuntimeError) as error:
            signing.check_secrets({"MACOS_CERTIFICATE_PASSWORD": "private-value"})
        self.assertIn("MACOS_CERTIFICATE_P12_BASE64", str(error.exception))
        self.assertNotIn("private-value", str(error.exception))

    def test_sensitive_command_failure_does_not_expose_arguments_or_output(self):
        failure = subprocess.CompletedProcess([], 1, "private-output", "private-password")
        with patch.object(signing.subprocess, "run", return_value=failure):
            with self.assertRaises(RuntimeError) as error:
                signing.run(["security", "import", "private-argument"], sensitive=True)
        self.assertNotIn("private-", str(error.exception))

    def test_all_macho_libraries_are_discovered_and_symlink_aliases_are_not_resigned(self):
        library = self.stage / "Bin/libSDL3.dylib"
        library.write_bytes(b"\xca\xfe\xba\xbe" + b"fixture")
        (self.stage / "Bin/alias.dylib").symlink_to(library.name)
        (self.stage / "Bin/config.ini").write_text("data")
        self.assertEqual(set(signing.macho_files(self.stage)), {self.player, library})

    def test_external_symlink_is_rejected(self):
        (self.stage / "Bin/external.dylib").symlink_to(self.root / "outside")
        with self.assertRaisesRegex(RuntimeError, "leaves stage"):
            signing.macho_files(self.stage)

    def test_adhoc_or_wrong_team_signature_is_rejected(self):
        with patch.object(signing, "run", return_value=subprocess.CompletedProcess([], 0, "", "Signature=adhoc\nTeamIdentifier=not set")):
            with self.assertRaisesRegex(RuntimeError, "Developer ID"):
                signing.verify_signatures(self.stage, "ABCDEFGHIJ")

    def test_runtime_in_path_does_not_count_as_hardened_runtime_flag(self):
        details = f"Executable={self.player}\nTeamIdentifier=ABCDEFGHIJ\nAuthority=Developer ID Application: Test\nTimestamp=now\nflags=0x0(none)\n"
        with patch.object(signing, "run", return_value=subprocess.CompletedProcess([], 0, "", details)):
            with self.assertRaisesRegex(RuntimeError, "Hardened Runtime"):
                signing.verify_signatures(self.stage, "ABCDEFGHIJ")

    def test_all_four_packages_are_required(self):
        self.packages()
        next(self.root.glob("*.dmg")).unlink()
        with self.assertRaisesRegex(RuntimeError, "All four"):
            publishing.release_files(self.root, self.revision)

    def test_changed_image_or_source_revision_prevents_publication(self):
        self.packages()
        self.assertEqual(len(publishing.release_files(self.root, self.revision)), 4)
        with self.assertRaisesRegex(RuntimeError, "manifest mismatch"):
            publishing.release_files(self.root, "b" * 40)
        next(self.root.glob("*.dmg")).write_bytes(b"tampered")
        with self.assertRaisesRegex(RuntimeError, "manifest mismatch"):
            publishing.release_files(self.root, self.revision)

    def test_moved_tag_prevents_upload(self):
        self.packages()
        with patch.object(publishing, "remote_tag", return_value="b" * 40), patch.object(publishing, "run") as command:
            with self.assertRaisesRegex(RuntimeError, "tag moved"):
                publishing.publish(self.root, "owner/repo", "v0.2.0", self.revision)
            command.assert_not_called()

    def test_rejected_notarization_does_not_staple_or_emit_manifest(self):
        work = self.root / "signing"
        work.mkdir()
        signing.write_json(work / "state.json", {"identity": "identity", "team": "ABCDEFGHIJ", "profile": "profile", "keychain": "keychain"})
        image = self.root / "release.dmg"
        def command(args, **kwargs):
            if "submit" in args:
                return subprocess.CompletedProcess(args, 1, json.dumps({"id": "rejected", "status": "Invalid"}), "")
            return subprocess.CompletedProcess(args, 0, "{}", "")
        with patch.object(signing, "verify_signatures"), patch.object(signing, "run", side_effect=command) as executed:
            with self.assertRaisesRegex(RuntimeError, "did not accept"):
                signing.notarize(self.stage, image, work, self.revision)
        self.assertFalse(any("stapler" in call.args[0] for call in executed.call_args_list))
        self.assertFalse(image.with_suffix(".json").exists())

    def test_failed_upload_preserves_existing_release_assets(self):
        self.packages()
        def command(args, **kwargs):
            if "upload" in args:
                raise RuntimeError("upload interrupted")
            return subprocess.CompletedProcess(args, 0, "", "")
        with patch.object(publishing, "remote_tag", return_value=self.revision), patch.object(publishing, "run", side_effect=command) as executed:
            with self.assertRaisesRegex(RuntimeError, "upload interrupted"):
                publishing.publish(self.root, "owner/repo", "v0.2.0", self.revision)
        self.assertFalse(any("delete-asset" in call.args[0] for call in executed.call_args_list))


if __name__ == "__main__":
    unittest.main()
