import json
from pathlib import Path
import re
import subprocess
import unittest

from app_ci import classify
from run_cmake_checks import selected_checks


ROOT = Path(__file__).resolve().parents[2]


class ApplicationContractsTest(unittest.TestCase):
    def test_player_and_engine_test_changes_do_not_build_a_runtime_package(self):
        player = classify(['Source/Player/tests/GameConfigTest.cpp'])
        self.assertFalse(player['build'])
        self.assertEqual(player['modules'], ['player'])
        self.assertEqual(player['checks'], [])
        engine = classify(['Source/NeMo2/Source/CK2/tests/CKRandomContextTest.cpp'])
        self.assertFalse(engine['build'])
        self.assertEqual(engine['modules'], [])
        self.assertTrue(classify(['Source/Player/tests/GameConfigTest.cpp', 'Source/Player/src/GamePlayer.cpp'])['build'])

    def test_registry_changes_run_the_corresponding_light_check(self):
        scope = classify(['cmake/VerifyComponentRegistry.cmake'])
        self.assertFalse(scope['build'])
        self.assertEqual(scope['modules'], [])
        self.assertEqual(selected_checks(ROOT, scope['checks']), ['ComponentRegistrySelection'])
        workflow = (ROOT / '.github/workflows/validate.yml').read_text()
        self.assertIn("needs.changes.outputs.light_checks != '[]'", workflow)
        self.assertIn("run_cmake_checks.py --checks '${{ needs.changes.outputs.light_checks }}'", workflow)

    def test_reusable_workflow_preserves_test_only_selection(self):
        scope = {'modules': ['player'], 'checks': [], 'build_runtime': False}
        output = subprocess.check_output(['python3', '-B', str(ROOT / '.github/scripts/app_ci.py'),
                                          '--selection', json.dumps(scope), '--target', 'sdl-macos-arm64-runtime'], text=True)
        result = json.loads(output)
        self.assertFalse(result['build'])
        self.assertTrue(result['compile'])
        self.assertEqual(json.loads(result['selection']), scope)

    def test_nightly_baseline_requests_only_successful_runs(self):
        workflow = (ROOT / '.github/workflows/build.yml').read_text()
        command = re.search(r'BASE=\$\((gh run list.*?)\)\n', workflow, re.S)[1]
        script = '''gh() {
          case " $* " in
            *" --status success "*) printf '%040d' 1;;
            *) printf '%040d' 2;;
          esac
        }
        ''' + 'BASE=$(' + command + ')\nprintf "%s" "$BASE"'
        self.assertEqual(subprocess.check_output(['bash', '-c', script], text=True), f'{1:040d}')


if __name__ == '__main__':
    unittest.main()
