"""Extract the production fragment source for an independent Metal replay."""

from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[1]
shaders = root / 'Source/RenderEngine/src/CKRasterizer/CKSdlGpuRasterizer/shaders'
sys.path.insert(0, str(shaders))
import shader_pack

names = re.findall(r'CKSDL_SHADER_(\w+),', (shaders / 'generated/shaders.h').read_text())
data = bytes(int(value, 16) for value in re.findall(
    r'0x([0-9a-f]{2})', (shaders / 'generated/msl_pack.h').read_text()))
artifacts = shader_pack.decode(data)
out = Path(sys.argv[1])
out.mkdir(parents=True, exist_ok=True)
for name in ('FS_FF_STAGE', 'FS_FF_STAGE_NATIVE',
             'FS_FF_STAGE_CUBE', 'FS_FF_STAGE_CUBE_NATIVE'):
    source = artifacts[names.index(name)].decode().rstrip('\0')
    fields = re.search(r'struct main0_in\s*\{(.*?)\};', source, re.S).group(1)
    fields = fields.replace(', flat', '')
    source += '\nstruct ProbeVertex {' + fields + 'float4 position [[position]]; };\n'
    source += 'vertex ProbeVertex vs(uint i [[vertex_id]]) { ProbeVertex v = {};\n'
    for index in range(14):
        value = 'float4(1)' if index in (0, 2) else 'float4(1,0,0,0)' if 4 <= index <= 11 else 'float2(0)' if index == 13 else 'float4(0)'
        source += f'v.in_var_TEXCOORD{index} = {value};\n'
    source += 'v.position=float4(i==1?3.0:-1.0,i==2?3.0:-1.0,0.5,1); return v; }\n'
    (out / (name.lower() + '.metal')).write_text(source)
