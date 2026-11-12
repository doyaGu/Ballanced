# Ballanced runtime

Ballanced assembles the Ballance application using BallancePlayer and a pinned
NeMo2 engine revision.

## Language

**Player**:
The Ballance application executable, including game configuration, startup,
static module registration, and the game loop.

**Engine runtime**:
The Virtools libraries, rasterizer, managers, readers, and building blocks
assembled by NeMo2. It excludes Player and original game assets.

**Game assets**:
The original Ballance data, including `base.cmo`, `Database.tdb`, textures,
sounds, text, and 3D entities.

**Stage**:
The installed runtime tree containing Player, the selected engine modules,
and any explicitly staged game assets.

Rendering terms and implementation guidance live in
[NeMo2](Source/NeMo2/CONTEXT.md) and its
[rendering document](Source/NeMo2/docs/RENDERING.md).
