#include <SDL3/SDL.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
__declspec(dllexport) extern const unsigned int D3D12SDKVersion = 620;
__declspec(dllexport) const char *D3D12SDKPath = ".\\D3D12\\";
}

#define PROBE_SDL_FUNCTIONS(M) \
    M(SDL_Init) M(SDL_Quit) M(SDL_GetError) M(SDL_ClearError) M(SDL_CreateWindow) M(SDL_DestroyWindow) \
    M(SDL_CreateProperties) M(SDL_DestroyProperties) M(SDL_SetBooleanProperty) \
    M(SDL_SetNumberProperty) M(SDL_SetStringProperty) M(SDL_CreateGPUDeviceWithProperties) \
    M(SDL_PumpEvents) M(SDL_CreateGPUDevice) M(SDL_DestroyGPUDevice) \
    M(SDL_SetGPUAllowedFramesInFlight) M(SDL_ClaimWindowForGPUDevice) \
    M(SDL_ReleaseWindowFromGPUDevice) M(SDL_SetGPUSwapchainParameters) \
    M(SDL_GetGPUSwapchainTextureFormat) M(SDL_CreateGPUTexture) M(SDL_ReleaseGPUTexture) \
    M(SDL_CreateGPUTransferBuffer) M(SDL_ReleaseGPUTransferBuffer) \
    M(SDL_AcquireGPUCommandBuffer) M(SDL_CancelGPUCommandBuffer) \
    M(SDL_WaitAndAcquireGPUSwapchainTexture) M(SDL_BeginGPURenderPass) M(SDL_EndGPURenderPass) \
    M(SDL_BeginGPUCopyPass) M(SDL_EndGPUCopyPass) M(SDL_CopyGPUTextureToTexture) \
    M(SDL_BlitGPUTexture) M(SDL_DownloadFromGPUTexture) \
    M(SDL_SubmitGPUCommandBufferAndAcquireFence) M(SDL_WaitForGPUFences) M(SDL_ReleaseGPUFence) \
    M(SDL_MapGPUTransferBuffer) M(SDL_UnmapGPUTransferBuffer)
#define DECLARE_SDL(name) static decltype(&name) p##name;
PROBE_SDL_FUNCTIONS(DECLARE_SDL)
#undef DECLARE_SDL

static bool ProbeWindow(int mode, bool debug, bool independent)
{
    SDL_Window *window = pSDL_CreateWindow("SDL GPU presentation probe", 640, 480, SDL_WINDOW_RESIZABLE);
    SDL_GPUDevice *device = NULL;
    if (independent) {
        char path[MAX_PATH];
        const DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
        if (!length || length >= MAX_PATH) {
            pSDL_DestroyWindow(window);
            return false;
        }
        char *directory = strrchr(path, '\\');
        if (!directory || strcpy_s(directory + 1, sizeof(path) - size_t(directory + 1 - path), "D3D12\\")) {
            pSDL_DestroyWindow(window);
            return false;
        }
        const SDL_PropertiesID props = pSDL_CreateProperties();
        const bool configured = props &&
            pSDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true) &&
            pSDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, debug) &&
            pSDL_SetStringProperty(props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "direct3d12") &&
            pSDL_SetNumberProperty(props, SDL_PROP_GPU_DEVICE_CREATE_D3D12_AGILITY_SDK_VERSION_NUMBER, 620) &&
            pSDL_SetStringProperty(props, SDL_PROP_GPU_DEVICE_CREATE_D3D12_AGILITY_SDK_PATH_STRING, path);
        if (configured) device = pSDL_CreateGPUDeviceWithProperties(props);
        if (props) pSDL_DestroyProperties(props);
    } else {
        device = pSDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, debug, "direct3d12");
    }
    SDL_GPUTexture *source = NULL;
    SDL_GPUTransferBuffer *download = NULL;
    SDL_GPUCommandBuffer *commands = NULL;
    SDL_GPUFence *fence = NULL;
    bool claimed = false;
    auto cleanup = [&]() {
        if (commands) pSDL_CancelGPUCommandBuffer(commands);
        if (fence) pSDL_ReleaseGPUFence(device, fence);
        if (source) pSDL_ReleaseGPUTexture(device, source);
        if (download) pSDL_ReleaseGPUTransferBuffer(device, download);
        if (claimed) pSDL_ReleaseWindowFromGPUDevice(device, window);
        if (device) pSDL_DestroyGPUDevice(device);
        if (window) pSDL_DestroyWindow(window);
    };
    auto fail = [&](const char *operation) {
        fprintf(stderr, "SDL present mode=%d %s: %s\n", mode, operation, pSDL_GetError());
        cleanup();
        return false;
    };
    if (!window || !device) return fail("create window/device");
    if (!pSDL_SetGPUAllowedFramesInFlight(device, 2) || !pSDL_ClaimWindowForGPUDevice(device, window))
        return fail("claim window");
    claimed = true;
    if (!pSDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC))
        return fail("swapchain parameters");
    const SDL_GPUTextureFormat format = pSDL_GetGPUSwapchainTextureFormat(device, window);
    if (format != SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM && format != SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM)
        return fail("unexpected SDR texture format");
    Uint32 width = 0, height = 0;
    for (unsigned frame = 0; frame < 5; ++frame) {
        pSDL_PumpEvents();
        commands = pSDL_AcquireGPUCommandBuffer(device);
        if (!commands) return fail("acquire commands");
        SDL_GPUTexture *swapchain = NULL;
        Uint32 w = 0, h = 0;
        if (!pSDL_WaitAndAcquireGPUSwapchainTexture(commands, window, &swapchain, &w, &h) || !swapchain)
            return fail("acquire swapchain");
        if (!source && mode < 4) {
            width = w; height = h;
            SDL_GPUTextureCreateInfo info = {};
            info.type = SDL_GPU_TEXTURETYPE_2D; info.format = format;
            info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
            info.width = width; info.height = height;
            info.layer_count_or_depth = info.num_levels = 1;
            info.sample_count = SDL_GPU_SAMPLECOUNT_1;
            source = pSDL_CreateGPUTexture(device, &info);
            SDL_GPUTransferBufferCreateInfo transfer = {SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD, width * height * 4, 0};
            download = pSDL_CreateGPUTransferBuffer(device, &transfer);
            if (!source || !download) return fail("create source/readback");
        }
        if (mode < 4 && (w != width || h != height)) return fail("unexpected swapchain resize");
        SDL_GPUColorTargetInfo target = {};
        target.texture = source;
        target.clear_color = {0.2f, 0.4f, 0.6f, 1.0f};
        target.load_op = SDL_GPU_LOADOP_CLEAR; target.store_op = SDL_GPU_STOREOP_STORE;
        SDL_GPURenderPass *pass = NULL;
        if (mode < 4) {
            pass = pSDL_BeginGPURenderPass(commands, &target, 1, NULL);
            if (!pass) return fail("begin offscreen clear");
            pSDL_EndGPURenderPass(pass);
        }
        if (mode == 0 || mode == 3 || mode == 4) {
            target.texture = swapchain;
            pass = pSDL_BeginGPURenderPass(commands, &target, 1, NULL);
            if (!pass) return fail("begin swapchain clear");
            pSDL_EndGPURenderPass(pass);
        } else if (mode == 1) {
            SDL_GPUCopyPass *copy = pSDL_BeginGPUCopyPass(commands);
            if (!copy) return fail("begin present copy");
            SDL_GPUTextureLocation from = {}, to = {};
            from.texture = source; to.texture = swapchain;
            pSDL_CopyGPUTextureToTexture(copy, &from, &to, width, height, 1, false);
            pSDL_EndGPUCopyPass(copy);
        } else if (mode == 2) {
            SDL_GPUBlitInfo blit = {};
            blit.source = {source, 0, 0, 0, 0, width, height};
            blit.destination = {swapchain, 0, 0, 0, 0, width, height};
            blit.load_op = SDL_GPU_LOADOP_DONT_CARE; blit.filter = SDL_GPU_FILTER_LINEAR;
            pSDL_BlitGPUTexture(commands, &blit);
        }
        if (mode < 3) {
            SDL_GPUCopyPass *copy = pSDL_BeginGPUCopyPass(commands);
            if (!copy) return fail("begin readback");
            SDL_GPUTextureRegion region = {};
            region.texture = source; region.w = width; region.h = height; region.d = 1;
            SDL_GPUTextureTransferInfo output = {download, 0, 0};
            pSDL_DownloadFromGPUTexture(copy, &region, &output);
            pSDL_EndGPUCopyPass(copy);
        }
        SDL_GPUCommandBuffer *submitted = commands;
        commands = NULL;
        pSDL_ClearError();
        fence = pSDL_SubmitGPUCommandBufferAndAcquireFence(submitted);
        if (!fence) return fail("submit");
        if (!pSDL_WaitForGPUFences(device, true, &fence, 1)) return fail("wait");
        if (mode >= 3) {
            pSDL_ReleaseGPUFence(device, fence);
            fence = NULL;
            continue;
        }
        const Uint8 *pixels = static_cast<const Uint8 *>(pSDL_MapGPUTransferBuffer(device, download, false));
        if (!pixels) return fail("map readback");
        const Uint8 first = format == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM ? 153 : 51;
        const Uint8 third = format == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM ? 51 : 153;
        bool exact = true;
        for (Uint32 i = 0; i < width * height; ++i)
            if (pixels[i * 4] != first || pixels[i * 4 + 1] != 102 ||
                pixels[i * 4 + 2] != third || pixels[i * 4 + 3] != 255) { exact = false; break; }
        pSDL_UnmapGPUTransferBuffer(device, download);
        if (!exact) return fail("incorrect clear pixels");
        pSDL_ReleaseGPUFence(device, fence);
        fence = NULL;
    }
    cleanup();
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5) return 1;
    printf("SDL process=%lu\n", GetCurrentProcessId());
    HMODULE library = LoadLibraryA(argv[1]);
    if (!library) return 2;
#define LOAD_SDL(name) p##name = reinterpret_cast<decltype(p##name)>(GetProcAddress(library, #name)); if (!p##name) return 3;
    PROBE_SDL_FUNCTIONS(LOAD_SDL)
#undef LOAD_SDL
    const int mode = atoi(argv[2]);
    if (mode < 0 || mode > 5 || !pSDL_Init(SDL_INIT_VIDEO)) return 4;
    bool passed = true;
    unsigned windows = 0;
    const bool independent = argc == 5 && atoi(argv[4]) != 0;
    for (; windows < 32 && passed; ++windows) passed = ProbeWindow(mode, atoi(argv[3]) != 0, independent);
    printf("SDL presents: mode=%d independent=%d windows=%u/32 success=%d\n", mode, independent, windows, passed);
    pSDL_Quit();
    FreeLibrary(library);
    return passed ? 0 : 5;
}
