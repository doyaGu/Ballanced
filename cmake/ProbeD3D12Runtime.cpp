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

static HRESULT ProbePresents(IDXGIFactory4 *factory, ID3D12Device *device, int windows, int fenceMode)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    ID3D12CommandQueue *queue = NULL;
    HRESULT result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue));
    if (FAILED(result)) return result;
    ID3D12Fence *fence = NULL;
    HANDLE event = NULL;
    UINT64 fenceValue = 0;
    if (fenceMode) {
        result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        if (FAILED(result)) { queue->Release(); return result; }
        event = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (!event) { fence->Release(); queue->Release(); return HRESULT_FROM_WIN32(GetLastError()); }
    }
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = DefWindowProcW;
    windowClass.hInstance = GetModuleHandleW(NULL);
    windowClass.lpszClassName = L"BallancedD3D12Probe";
    if (!RegisterClassW(&windowClass)) {
        const HRESULT error = HRESULT_FROM_WIN32(GetLastError());
        if (event) CloseHandle(event);
        if (fence) fence->Release();
        queue->Release();
        return error;
    }
    unsigned presented = 0;
    for (int i = 0; i < windows && SUCCEEDED(result); ++i) {
        HWND window = CreateWindowW(windowClass.lpszClassName, L"Direct3D presentation probe",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
            NULL, NULL, windowClass.hInstance, NULL);
        if (!window) { result = HRESULT_FROM_WIN32(GetLastError()); break; }
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_NONE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        IDXGISwapChain1 *swapchain = NULL;
        result = factory->CreateSwapChainForHwnd(queue, window, &desc, NULL, NULL, &swapchain);
        for (unsigned frame = 0; frame < 5 && SUCCEEDED(result); ++frame) {
            MSG message;
            while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (fence) {
                if (fenceMode == 1) {
                    result = fence->Signal(0);
                    fenceValue = 1;
                } else ++fenceValue;
                if (SUCCEEDED(result)) result = queue->Signal(fence, fenceValue);
                if (FAILED(result)) break;
            }
            result = swapchain->Present(1, 0);
            if (result == S_OK) ++presented;
            else printf("Present window=%d frame=%u: 0x%08lx\n", i, frame, result);
            if (fence && fence->GetCompletedValue() < fenceValue) {
                const HRESULT wait = fence->SetEventOnCompletion(fenceValue, event);
                if (FAILED(wait)) result = wait;
                else if (WaitForSingleObject(event, 5000) != WAIT_OBJECT_0) result = E_FAIL;
            }
        }
        if (swapchain) swapchain->Release();
        DestroyWindow(window);
    }
    printf("Native DXGI presents: fence=%d %u/%d result=0x%08lx\n", fenceMode, presented, windows * 5, result);
    UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
    if (event) CloseHandle(event);
    if (fence) fence->Release();
    queue->Release();
    return SUCCEEDED(result) && presented != unsigned(windows * 5) ? E_FAIL : result;
}

int main(int argc, char **argv)
{
    if (argc > 2 && atoi(argv[2]) <= 0) return 1;
    printf("Native process=%lu\n", GetCurrentProcessId());
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
    if (FAILED(result)) { factory->Release(); return 4; }
    ID3D12Device *device = NULL;
    result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    adapter->Release();
    printf("D3D12CreateDevice(debug=%d): 0x%08lx\n", debug, result);
    if (SUCCEEDED(result) && argc > 2)
        result = ProbePresents(factory, device, atoi(argv[2]), argc > 3 ? atoi(argv[3]) : 0);
    factory->Release();
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
