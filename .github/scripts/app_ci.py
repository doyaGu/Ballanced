"""Select application checks without rerunning engine test suites."""

import argparse
import hashlib
import json
from pathlib import Path
import sys

ENGINE_SCRIPTS = Path(__file__).resolve().parents[2] / 'Source/NeMo2/.github/scripts'
sys.path.insert(0, str(ENGINE_SCRIPTS))
from change_detection import baseline, changed_paths, documentation, emit
from engine_ci import classify as engine_changes


def classify(paths):
    paths = list(paths)
    engine = engine_changes(path.removeprefix('Source/NeMo2/') for path in paths
                            if path.startswith('Source/NeMo2/'))
    modules, scripts, checks = set(), set(), set()
    layout = False
    for path in paths:
        if documentation(path):
            continue
        if path == 'Source/Player' or path.startswith('Source/Player/'):
            modules.add('player')
            checks.update(['ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'])
            if path.endswith(('.cmake', 'CMakeLists.txt')) or 'StaticPlugin' in Path(path).name:
                modules.add('assembly')
                layout = True
        elif path == 'Source/NeMo2':
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
        elif path.endswith(('.cmake', 'CMakeLists.txt')) or path in {'CMakePresets.json', '.gitmodules'}:
            modules.add('assembly')
            layout = True
    if engine['build']:
        modules.add('assembly')
        layout |= engine['layout']
    return dict(modules=sorted(modules), checks=sorted(checks), scripts=sorted(scripts),
                build=bool(modules), layout=layout)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', default='')
    parser.add_argument('--head', default='HEAD')
    parser.add_argument('--modules', default='changed')
    parser.add_argument('--selection', default='')
    args = parser.parse_args()
    if args.selection:
        result = dict(json.loads(args.selection), scripts=[], layout=False, build=True)
    else:
        result = classify(changed_paths(Path.cwd(), baseline(Path.cwd(), args.base), args.head))
        if args.modules != 'changed':
            result['modules'] = ['assembly', 'player'] if args.modules == 'all' else [args.modules]
    if set(result['modules']) - {'assembly', 'player'}:
        raise ValueError('unknown application test module')
    if 'player' in result['modules']:
        result['checks'] = sorted(set(result['checks']) | {'ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'})
    if set(result['checks']) - {'ApplicationNumericalIsolation', 'PlayerSdlShortcutsTest'}:
        raise ValueError('unknown application check')
    scope = dict(modules=result['modules'], checks=result['checks'])
    encoded = json.dumps(scope, separators=(',', ':'))
    result.update(selection=encoded, test=bool(scope['modules'] or scope['checks']), compile=bool(scope['modules']),
                  cache_scope=hashlib.sha256(encoded.encode()).hexdigest()[:12])
    emit(result)


if __name__ == '__main__':
    main()
