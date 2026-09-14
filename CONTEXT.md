# Ballanced Rendering

Ballanced keeps Virtools rendering behavior while implementing it directly on modern graphics APIs.

## Language

**Virtools-Style Rasterizer Interface**:
The private C++ surface built around `CKRasterizer`, `CKRasterizerDriver`, and `CKRasterizerContext`. It keeps the original class model and state/resource/draw vocabulary, but may improve method shapes when correctness or performance benefits.
_Avoid_: exact legacy clone, public SDK promise

The three classes are implementation-bearing bases, not pure interface-only classes. Rasterizer/driver ownership, enumeration, descriptions, and ordinary Context bookkeeping stay in their base implementation so concrete rasterizers only repeat behavior that genuinely differs.

**Concrete Rasterizer Context**:
The complete bgfx or SDL_GPU `CKRasterizerContext` implementation. It owns lifecycle, resources, native handles, transfers, presentation, and native command submission. It contains the fixed-function implementation directly; it does not sit behind a second graphics interface.
_Avoid_: backend adapter, provider, translation wrapper

**CKFFPLib component**:
Reusable fixed-function implementation used by each Concrete Rasterizer Context. It owns canonical render state, Virtools state interaction rules, draw validation, shader keys and constants, and CPU vertex/index conversion. It returns resolved fixed-function draw data but never owns a GPU device, creates native objects, submits commands, or implements `CKRasterizerContext`.
_Avoid_: backend-independent rasterizer, second rasterizer interface, context façade

**Rasterizer Handle**:
The opaque caller-visible `CKDWORD` identity returned by typed resource creation. Zero is invalid, and callers must stop using a handle after successful deletion. The Concrete Rasterizer Context validates kind and liveness before resolving its own native resource; allocation and reuse policy are implementation details.
_Avoid_: legacy object index, raw native handle, native resource id

**Original Feature Baseline**:
The observable behavior represented by the original `CKRasterizer` methods, object types, state operations, and capability reporting. An entry is complete only when both Concrete Rasterizer Contexts implement it, `CKFFPLib` supplies the shared fixed-function behavior, or capabilities explicitly report it unsupported. Silent no-ops are not complete.
_Avoid_: legacy feature list, best-effort compatibility

## Implementation direction

```text
CKBgfxRasterizerContext ─┐
                        ├── owns a CKFFPLib component
CKSdlGpuRasterizerContext┘
```

There is no production `CKBackend*` interface between a context and its graphics API. Shared code is ordinary state/conversion/cache code with explicit inputs and outputs. Native resource creation, command encoding, synchronization, and presentation stay in the concrete context so hot calls are direct and backend-specific facilities remain usable.

## Example dialogue

> Developer: Should fixed-function state be duplicated in the bgfx and SDL_GPU contexts?
>
> Rendering maintainer: No. Both contexts contain the same CKFFPLib component and consume its resolved draw result.
>
> Developer: Should we add a generic device interface so CKFFPLib can submit the result?
>
> Rendering maintainer: No. The concrete context resolves its native program, layout, resources, and submits directly.
