import unittest

from app_ci import classify


class ApplicationSelectionTest(unittest.TestCase):
    def test_nested_engine_documentation_does_not_run_application_tests(self):
        result = classify(['Source/NeMo2/README.md', 'Source/NeMo2/LICENSES/notice.txt', 'README.md'])
        self.assertFalse(result['build'])
        self.assertEqual(result['modules'], [])

    def test_player_source_selects_only_player_units(self):
        result = classify(['Source/Player/src/GamePlayer.cpp'])
        self.assertEqual(result['modules'], ['player'])
        self.assertEqual(result['checks'], ['ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'])

    def test_engine_change_selects_assembly_without_player_units(self):
        result = classify(['Source/NeMo2/Source/VxMath/src/VxVector.cpp'])
        self.assertEqual(result['modules'], ['assembly'])

    def test_player_tools_do_not_build_or_test_engine(self):
        result = classify(['tests/player/player_session.py'])
        self.assertEqual(result['scripts'], ['player_sessions'])
        self.assertFalse(result['build'])
        self.assertEqual(result['modules'], [])

    def test_release_tool_changes_do_not_run_player_tests(self):
        result = classify(['.github/scripts/macos_release.py'])
        self.assertEqual(result['scripts'], ['release'])
        self.assertEqual(result['modules'], [])


if __name__ == '__main__':
    unittest.main()
