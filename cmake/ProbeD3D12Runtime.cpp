#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(PROBE_D3D12_SDK_VERSION)
extern "C" {
__declspec(dllexport) extern const UINT D3D12SDKVersion = PROBE_D3D12_SDK_VERSION;
__declspec(dllexport) const char *D3D12SDKPath = ".\\D3D12\\";
}
#endif

int main(int argc, char **argv)
{
    const bool debug = argc > 1 && atoi(argv[1]) != 0;
    if (debug) {
        ID3D12Debug *layer = NULL;
        const HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&layer));
        printf("D3D12 debug layer: 0x%08lx\n", result);
        if (FAILED(result)) return 2;
        layer->EnableDebugLayer();
        layer->Release();
    }
    IDXGIFactory4 *factory = NULL;
    HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(result)) return 3;
    IDXGIAdapter *adapter = NULL;
    result = factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
    factory->Release();
    if (FAILED(result)) return 4;
    ID3D12Device *device = NULL;
    result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    adapter->Release();
    printf("D3D12CreateDevice(debug=%d): 0x%08lx\n", debug, result);
    const char *modules[] = {"D3D12Core.dll", "d3d12SDKLayers.dll", "d3d10warp.dll"};
    for (unsigned i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i) {
        char path[MAX_PATH];
        const HMODULE module = GetModuleHandleA(modules[i]);
        if (module && GetModuleFileNameA(module, path, MAX_PATH))
            printf("%s: %s\n", modules[i], path);
    }
    if (device) device->Release();
    return SUCCEEDED(result) ? 0 : 5;
}
