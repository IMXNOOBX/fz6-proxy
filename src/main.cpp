#include <windows.h>
#include <d3d12.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <string>

#include "misc/logger.hpp"

static HMODULE g_originalD3D12 = nullptr;

using D3D12CreateDevice_t = HRESULT(WINAPI*)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);
using CheckFeatureSupport_t = HRESULT(STDMETHODCALLTYPE*)(ID3D12Device*, D3D12_FEATURE, void*, UINT);

static D3D12CreateDevice_t g_originalD3D12CreateDevice = nullptr;
static CheckFeatureSupport_t g_originalCheckFeatureSupport = nullptr;
static LONG g_featureLogBudget = 80;

static constexpr size_t kCheckFeatureSupportVtableIndex = 13;

static const char* FeatureLevelName(D3D_FEATURE_LEVEL level) {
    switch (level) {
    case D3D_FEATURE_LEVEL_12_1: return "12_1";
    case D3D_FEATURE_LEVEL_12_0: return "12_0";
    case D3D_FEATURE_LEVEL_11_1: return "11_1";
    case D3D_FEATURE_LEVEL_11_0: return "11_0";
    default: return "UNKNOWN";
    }
}

static const char* FeatureName(D3D12_FEATURE feature) {
    switch (feature) {
    case D3D12_FEATURE_FEATURE_LEVELS: return "FEATURE_LEVELS";
    case D3D12_FEATURE_SHADER_MODEL: return "SHADER_MODEL";
    case D3D12_FEATURE_D3D12_OPTIONS: return "OPTIONS";
    case D3D12_FEATURE_D3D12_OPTIONS1: return "OPTIONS1";
    case D3D12_FEATURE_D3D12_OPTIONS2: return "OPTIONS2";
    case D3D12_FEATURE_D3D12_OPTIONS3: return "OPTIONS3";
    case D3D12_FEATURE_D3D12_OPTIONS4: return "OPTIONS4";
    case D3D12_FEATURE_D3D12_OPTIONS5: return "OPTIONS5";
    case D3D12_FEATURE_D3D12_OPTIONS6: return "OPTIONS6";
    case D3D12_FEATURE_D3D12_OPTIONS7: return "OPTIONS7";
    case D3D12_FEATURE_D3D12_OPTIONS8: return "OPTIONS8";
    case D3D12_FEATURE_D3D12_OPTIONS9: return "OPTIONS9";
    case D3D12_FEATURE_D3D12_OPTIONS10: return "OPTIONS10";
    case D3D12_FEATURE_D3D12_OPTIONS11: return "OPTIONS11";
    case D3D12_FEATURE_D3D12_OPTIONS12: return "OPTIONS12";
    case D3D12_FEATURE_D3D12_OPTIONS13: return "OPTIONS13";
    default: return "UNKNOWN";
    }
}

static bool LoadOriginalD3D12() {
    if (g_originalD3D12CreateDevice)
        return true;

    char systemPath[MAX_PATH]{};
    if (!GetSystemDirectoryA(systemPath, MAX_PATH)) {
        LOG(FATAL, "Failed to get system path");
        return false;
    }

    std::string dllPath = std::string(systemPath) + "\\d3d12.dll";
    g_originalD3D12 = LoadLibraryA(dllPath.c_str());
    if (!g_originalD3D12) {
        LOG(FATAL, "Failed to locate original d3d12.dll library");
        return false;
    }

    g_originalD3D12CreateDevice = reinterpret_cast<D3D12CreateDevice_t>(GetProcAddress(g_originalD3D12, "D3D12CreateDevice"));

    if (!g_originalD3D12CreateDevice) {
        LOG(FATAL, "Failed to get original D3D12CreateDevice function");
        return false;
    }

    return true;
}

extern "C" FARPROC WINAPI GetOriginalProcByName(const char* name) {
    if (!LoadOriginalD3D12()) 
        return nullptr;
    
    return GetProcAddress(g_originalD3D12, name);
}

extern "C" FARPROC WINAPI GetOriginalProcByOrdinal(WORD ordinal) {
    if (!LoadOriginalD3D12()) 
        return nullptr;
    
    return GetProcAddress(g_originalD3D12, reinterpret_cast<LPCSTR>(ordinal));
}

static HRESULT STDMETHODCALLTYPE CheckFeatureSupportHook(
    ID3D12Device* device,
    D3D12_FEATURE feature,
    void* support_data,
    UINT data_size
) {
    CheckFeatureSupport_t original = g_originalCheckFeatureSupport;

    if (!original) 
        return E_FAIL;

    HRESULT hr = original(device, feature, support_data, data_size);

    if (InterlockedDecrement(&g_featureLogBudget) >= 0 || FAILED(hr)) {
        LOGF(DEBUG, "CheckFeatureSupport " << FeatureName(feature) << " ("
            << static_cast<unsigned>(feature)  << "): HRESULT 0x"  << std::uppercase << std::hex
            << std::setw(8) << std::setfill('0') << static_cast<unsigned long>(hr)
        );
    }

    if (!support_data)
        return hr;

    if (feature == D3D12_FEATURE_FEATURE_LEVELS && data_size >= sizeof(D3D12_FEATURE_DATA_FEATURE_LEVELS)) {
        auto* levels = reinterpret_cast<D3D12_FEATURE_DATA_FEATURE_LEVELS*>(support_data);
        levels->MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_12_1;
        LOG(SUCCESS, "Spoofing FEATURE_LEVELS HRESULT -> S_OK, MaxSupportedFeatureLevel -> 12_1");
        return S_OK;
    }

    if (feature == D3D12_FEATURE_SHADER_MODEL && data_size >= sizeof(D3D12_FEATURE_DATA_SHADER_MODEL)) {
        auto* shaderModel = reinterpret_cast<D3D12_FEATURE_DATA_SHADER_MODEL*>(support_data);
        (void)shaderModel;
        LOG(INFO, "Spoof skipped, SHADER_MODEL left native");
        return hr;
    }

    if (feature == D3D12_FEATURE_D3D12_OPTIONS7 && data_size >= sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7)) {
        auto* opts7 = reinterpret_cast<D3D12_FEATURE_DATA_D3D12_OPTIONS7*>(support_data);
        (void)opts7;
        LOG(INFO, "Spoof skipped, OPTIONS7 left native");
        return hr;
    }

    if (feature == D3D12_FEATURE_D3D12_OPTIONS12 && data_size >= sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS12)) {
        auto* opts12 = reinterpret_cast<D3D12_FEATURE_DATA_D3D12_OPTIONS12*>(support_data);
        opts12->MSPrimitivesPipelineStatisticIncludesCulledPrimitives = D3D12_TRI_STATE::D3D12_TRI_STATE_TRUE; // TRUE;
        LOG(SUCCESS, "Spoofing OPTIONS12 -> S_OK, MSPrimitivesPipelineStatisticIncludesCulledPrimitives -> D3D12_TRI_STATE_TRUE");
        return S_OK;
    }

    return hr;
}

static void PatchDeviceCheckFeatureSupport(IUnknown* unknown_device) {
    if (!unknown_device)
        return;

    ID3D12Device* device = nullptr;

    const HRESULT hr = unknown_device->QueryInterface(IID_PPV_ARGS(&device));
    if (FAILED(hr) || !device) {
        LOG(FATAL, "Failed to obtain ID3D12Device for CheckFeatureSupport patching");
        return;
    }

    const auto release_device = [&]() {
        device->Release();
        device = nullptr;
    };

    void*** vtable_ptr = reinterpret_cast<void***>(device);
    if (!vtable_ptr || !*vtable_ptr) {
        LOG(FATAL, "Invalid vtable pointer for ID3D12Device");
        release_device();
        return;
    }

    void** vtable = *vtable_ptr;
    void*& target_entry = vtable[kCheckFeatureSupportVtableIndex];

    const void* hook_ptr = reinterpret_cast<void*>(&CheckFeatureSupportHook);

    if (target_entry == hook_ptr) {
        LOG(INFO, "CheckFeatureSupport already hooked");
        release_device();
        return;
    }

    if (!g_originalCheckFeatureSupport) 
        g_originalCheckFeatureSupport = reinterpret_cast<CheckFeatureSupport_t>(target_entry);

    DWORD old_protect = 0;

    if (!VirtualProtect(
        &target_entry,
        sizeof(void*),
        PAGE_EXECUTE_READWRITE,
        &old_protect
    )) {
        LOG(FATAL, "VirtualProtect failed while patching CheckFeatureSupport");
        release_device();
        return;
    }

    target_entry = const_cast<void*>(hook_ptr);

    DWORD unused = 0;

    VirtualProtect( &target_entry, sizeof(void*), old_protect, &unused);

    FlushInstructionCache(GetCurrentProcess(), &target_entry, sizeof(void*));

    LOG(INFO, "Patched ID3D12Device::CheckFeatureSupport successfully");
    release_device();
}

extern "C" HRESULT WINAPI D3D12CreateDevice(
    IUnknown* adapter,
    D3D_FEATURE_LEVEL minimum_feature_level,
    REFIID riid,
    void** device_out
) {
    LOG(DEBUG, "Proxy intercepted call to D3D12CreateDevice");

    if (!LoadOriginalD3D12()) {
        if (device_out) 
            *device_out = nullptr;
        return E_FAIL;
    }

    if (!device_out) {
        return g_originalD3D12CreateDevice(
            adapter,
            minimum_feature_level,
            riid,
            device_out
        );
    }

    static constexpr D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    HRESULT last_hr = E_FAIL;

    for (const auto level : feature_levels) {
        *device_out = nullptr;

        last_hr = g_originalD3D12CreateDevice(
            adapter,
            level,
            riid,
            device_out
        );

        LOGF(DEBUG,  "D3D12CreateDevice attempt with FL "
            << FeatureLevelName(level)
            << " -> HRESULT 0x"
            << std::uppercase
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << static_cast<unsigned long>(last_hr)
        );

        if (SUCCEEDED(last_hr)) {
            LOG(INFO, "D3D12CreateDevice succeeded, patching device support");

            if (*device_out) 
                PatchDeviceCheckFeatureSupport(reinterpret_cast<IUnknown*>(*device_out));
            return last_hr;
        }
    }

    if (device_out) 
        *device_out = nullptr;

    LOG(FATAL, "D3D12CreateDevice failed for all feature levels");
    return last_hr;
}

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
    #ifdef _DEBUG
        LOG_INIT();
        LOG_OUT("d3d12-proxy.log") // The game seems to have a runtime check that crashes on unverified files
    #endif
        LOG(INFO, "DLL_PROCESS_ATTACH: d3d12.dll proxy loaded!");
    }

    return TRUE;
}
