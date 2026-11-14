"""Select application checks without rerunning engine test suites."""

import argparse
import hashlib
import json
from pathlib import Path
import sys

ENGINE_SCRIPTS = Path(__file__).resolve().parents[2] / 'Source/NeMo2/.github/scripts'
sys.path.insert(1, str(ENGINE_SCRIPTS))
from change_detection import baseline, changed_paths, documentation, emit
from engine_ci import classify as engine_changes, job_matrix
from run_cmake_checks import selected_checks

CHECKS = {'ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest',
          'ComponentRegistrySelection', 'StageInstallScheduling', 'CMakeRegeneration'}


def classify(paths):
    paths = list(paths)
    engine = engine_changes(path.removeprefix('Source/NeMo2/') for path in paths
                            if path.startswith('Source/NeMo2/'))
    modules, scripts, checks = set(), set(), set()
    layout = False
    application_build = False
    for path in paths:
        if documentation(path):
            continue
        if path == 'Source/Player' or path.startswith('Source/Player/'):
            modules.add('player')
            test_only = path.startswith('Source/Player/tests/')
            application_build |= not test_only
            if not test_only:
                checks.update(['ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'])
            if not test_only and (path.endswith(('.cmake', 'CMakeLists.txt')) or 'StaticPlugin' in Path(path).name):
                modules.add('assembly')
                layout = True
        elif path == 'Source/NeMo2':
            application_build = True
            modules.add('assembly')
            layout = True
        elif path.startswith('Source/NeMo2/'):
            if Path(path).name in {'engine_ci.py', 'change_detection.py', 'ctest_selection.py'}:
                scripts.add('ci')
        elif path.startswith('tests/player/') or path == '.github/scripts/test_player_sessions.py':
            scripts.add('player_sessions')
        elif (path.startswith('.github/scripts/') and 'macos_release' in Path(path).name) or path == '.github/workflows/macos-release.yml':
            scripts.add('release')
        elif path.startswith('.github/'):
            scripts.add('ci')
        elif path in {'cmake/VerifyComponentRegistry.cmake', 'cmake/TestComponentRegistrySelection.cmake'}:
            checks.add('ComponentRegistrySelection')
        elif path == 'cmake/TestStageInstallScheduling.cmake':
            checks.add('StageInstallScheduling')
        elif path == 'cmake/TestBuildRegeneration.cmake':
            checks.add('CMakeRegeneration')
        elif path.endswith(('.cmake', 'CMakeLists.txt')) or path in {'CMakePresets.json', '.gitmodules'}:
            application_build = True
            modules.add('assembly')
            layout = True
            checks.add('ComponentRegistrySelection')
            if path == 'cmake/BallanceStage.cmake':
                checks.add('StageInstallScheduling')
    if engine['build']:
        modules.add('assembly')
        layout |= engine['layout']
    return dict(modules=sorted(modules), checks=sorted(checks), scripts=sorted(scripts),
                build=application_build or engine['build'], layout=layout,
                resources_only=engine.get('resources_only', False) and not application_build)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', default='')
    parser.add_argument('--head', default='HEAD')
    parser.add_argument('--modules', default='changed')
    parser.add_argument('--selection', default='')
    parser.add_argument('--target', default='all')
    args = parser.parse_args()
    if args.selection:
        result = dict(json.loads(args.selection), scripts=[], layout=False)
        result['build'] = result.get('build_runtime', False)
    else:
        result = classify(changed_paths(Path.cwd(), baseline(Path.cwd(), args.base, '.github/scripts/app_ci.py'), args.head))
        if args.modules != 'changed':
            result['modules'] = ['assembly', 'player'] if args.modules == 'all' else [args.modules]
            result['checks'] = ['ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'] if args.modules in {'all', 'player'} else []
            result['resources_only'] = False
            result['build'] = True
    if set(result['modules']) - {'assembly', 'player'}:
        raise ValueError('unknown application test module')
    if set(result['checks']) - CHECKS:
        raise ValueError('unknown application check')
    if not isinstance(result['build'], bool):
        raise ValueError('build_runtime must be boolean')
    scope = dict(modules=result['modules'], checks=result['checks'], build_runtime=result['build'])
    if result.get('resources_only'):
        scope['resources_only'] = True
    encoded = json.dumps(scope, separators=(',', ':'))
    result.update(selection=encoded, test=bool(scope['modules'] or scope['checks']), compile=bool(scope['modules']),
                  light_checks=selected_checks(Path(__file__).resolve().parents[2], scope['checks']),
                  cache_scope=hashlib.sha256(encoded.encode()).hexdigest()[:12],
                  matrix=job_matrix(args.target, result.get('resources_only', False) and args.target == 'all'))
    emit(result)


if __name__ == '__main__':
    main()
