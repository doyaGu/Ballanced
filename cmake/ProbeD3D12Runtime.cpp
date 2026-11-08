#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(PROBE_SDL_WINDOW)
#include <SDL3/SDL.h>
#define PROBE_SDL_FUNCTIONS(M) \
    M(SDL_Init) M(SDL_Quit) M(SDL_CreateWindow) M(SDL_DestroyWindow) \
    M(SDL_GetWindowProperties) M(SDL_GetPointerProperty) M(SDL_PumpEvents)
#define DECLARE_SDL(name) static decltype(&name) p##name;
PROBE_SDL_FUNCTIONS(DECLARE_SDL)
#undef DECLARE_SDL
#endif

#if defined(PROBE_D3D12_SDK_VERSION)
extern "C" {
__declspec(dllexport) extern const UINT D3D12SDKVersion = PROBE_D3D12_SDK_VERSION;
__declspec(dllexport) const char *D3D12SDKPath = ".\\D3D12\\";
}
#endif

static HRESULT SelectAdapter(IDXGIFactory4 *factory, bool preference, IDXGIAdapter **adapter)
{
    if (!preference) return factory->EnumWarpAdapter(IID_PPV_ARGS(adapter));
    IDXGIFactory6 *factory6 = NULL;
    HRESULT result = factory->QueryInterface(IID_PPV_ARGS(&factory6));
    if (SUCCEEDED(result)) {
        result = factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter));
        factory6->Release();
    }
    return result;
}

static HRESULT ProbePresents(IDXGIFactory4 *factory, ID3D12Device *device, int windows, int fenceMode, bool clear, bool tearing, bool association)
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
#if defined(PROBE_SDL_WINDOW)
        SDL_Window *sdlWindow = pSDL_CreateWindow("Native DXGI on SDL window", 640, 480, SDL_WINDOW_RESIZABLE);
        HWND window = sdlWindow ? static_cast<HWND>(pSDL_GetPointerProperty(
            pSDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL)) : NULL;
#else
        HWND window = CreateWindowW(windowClass.lpszClassName, L"Direct3D presentation probe",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
            NULL, NULL, windowClass.hInstance, NULL);
#endif
        if (!window) { result = HRESULT_FROM_WIN32(GetLastError()); break; }
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_NONE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        IDXGISwapChain1 *swapchain = NULL;
        result = factory->CreateSwapChainForHwnd(queue, window, &desc, NULL, NULL, &swapchain);
        if (association && SUCCEEDED(result)) {
            IDXGIFactory1 *parent = NULL;
            result = swapchain->GetParent(IID_PPV_ARGS(&parent));
            if (SUCCEEDED(result)) {
                result = parent->MakeWindowAssociation(window, DXGI_MWA_NO_WINDOW_CHANGES);
                parent->Release();
            }
        }
        IDXGISwapChain3 *swapchain3 = NULL;
        ID3D12CommandAllocator *allocator = NULL;
        ID3D12GraphicsCommandList *commands = NULL;
        ID3D12DescriptorHeap *heap = NULL;
        if (clear && SUCCEEDED(result)) {
            result = swapchain->QueryInterface(IID_PPV_ARGS(&swapchain3));
            if (SUCCEEDED(result)) result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
            if (SUCCEEDED(result)) result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&commands));
            if (SUCCEEDED(result)) result = commands->Close();
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NumDescriptors = 1;
            if (SUCCEEDED(result)) result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));
        }
        for (unsigned frame = 0; frame < 5 && SUCCEEDED(result); ++frame) {
#if defined(PROBE_SDL_WINDOW)
            pSDL_PumpEvents();
#else
            MSG message;
            while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
#endif
            if (clear) {
                ID3D12Resource *buffer = NULL;
                result = swapchain3->GetBuffer(swapchain3->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&buffer));
                if (SUCCEEDED(result)) result = allocator->Reset();
                if (SUCCEEDED(result)) result = commands->Reset(allocator, NULL);
                if (SUCCEEDED(result)) {
                    const D3D12_CPU_DESCRIPTOR_HANDLE target = heap->GetCPUDescriptorHandleForHeapStart();
                    device->CreateRenderTargetView(buffer, NULL, target);
                    D3D12_RESOURCE_BARRIER barrier = {};
                    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    barrier.Transition.pResource = buffer;
                    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                    commands->ResourceBarrier(1, &barrier);
                    const float color[] = {0.2f, 0.4f, 0.6f, 1.0f};
                    commands->ClearRenderTargetView(target, color, 0, NULL);
                    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                    commands->ResourceBarrier(1, &barrier);
                    result = commands->Close();
                    if (SUCCEEDED(result)) {
                        ID3D12CommandList *submitted = commands;
                        queue->ExecuteCommandLists(1, &submitted);
                    }
                }
                if (buffer) buffer->Release();
                if (FAILED(result)) break;
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
        if (heap) heap->Release();
        if (commands) commands->Release();
        if (allocator) allocator->Release();
        if (swapchain3) swapchain3->Release();
        if (swapchain) swapchain->Release();
#if defined(PROBE_SDL_WINDOW)
        pSDL_DestroyWindow(sdlWindow);
#else
        DestroyWindow(window);
#endif
    }
    printf("Native DXGI presents: fence=%d clear=%d tearing=%d %u/%d result=0x%08lx\n", fenceMode, clear, tearing, presented, windows * 5, result);
    UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
    if (event) CloseHandle(event);
    if (fence) fence->Release();
    queue->Release();
    return SUCCEEDED(result) && presented != unsigned(windows * 5) ? E_FAIL : result;
}

int main(int argc, char **argv)
{
#if defined(PROBE_SDL_WINDOW)
    if (argc != 8 && argc != 10) return 1;
    const HMODULE library = LoadLibraryA(argv[7]);
    if (!library) return 1;
#define LOAD_SDL(name) p##name = reinterpret_cast<decltype(p##name)>(GetProcAddress(library, #name)); if (!p##name) return 1;
    PROBE_SDL_FUNCTIONS(LOAD_SDL)
#undef LOAD_SDL
    if (!pSDL_Init(SDL_INIT_VIDEO)) return 1;
#endif
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
    const bool preference = argc > 8 && atoi(argv[8]) != 0;
    result = SelectAdapter(factory, preference, &adapter);
    if (FAILED(result)) { factory->Release(); return 4; }
    DXGI_ADAPTER_DESC adapterDesc = {};
    if (SUCCEEDED(adapter->GetDesc(&adapterDesc)))
        printf("Adapter preference=%d luid=%08lx:%08lx name=%ls\n", preference,
            adapterDesc.AdapterLuid.HighPart, adapterDesc.AdapterLuid.LowPart, adapterDesc.Description);
    ID3D12Device *device = NULL;
    result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    adapter->Release();
    printf("D3D12CreateDevice(debug=%d): 0x%08lx\n", debug, result);
    if (SUCCEEDED(result) && argc > 2) {
        const int windows = atoi(argv[2]);
        const bool recreate = argc > 6 && atoi(argv[6]) != 0;
        for (int i = 0; i < (recreate ? windows : 1) && SUCCEEDED(result); ++i) {
            if (recreate && i) {
                device->Release(); device = NULL;
                IDXGIAdapter *warp = NULL;
                result = SelectAdapter(factory, preference, &warp);
                if (SUCCEEDED(result)) result = D3D12CreateDevice(warp, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
                if (warp) warp->Release();
            }
            if (SUCCEEDED(result)) result = ProbePresents(factory, device, recreate ? 1 : windows,
                argc > 3 ? atoi(argv[3]) : 0, argc > 4 && atoi(argv[4]) != 0,
                argc > 5 && atoi(argv[5]) != 0, argc > 9 && atoi(argv[9]) != 0);
        }
    }
    factory->Release();
    const char *modules[] = {"D3D12Core.dll", "d3d12SDKLayers.dll", "d3d10warp.dll"};
    for (unsigned i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i) {
        char path[MAX_PATH];
        const HMODULE module = GetModuleHandleA(modules[i]);
        if (module && GetModuleFileNameA(module, path, MAX_PATH))
            printf("%s: %s\n", modules[i], path);
    }
    if (device) device->Release();
#if defined(PROBE_SDL_WINDOW)
    pSDL_Quit();
    FreeLibrary(library);
#endif
    return SUCCEEDED(result) ? 0 : 5;
}
