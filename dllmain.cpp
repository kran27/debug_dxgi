#include "global.h"
#include "hooks.h"
#include "patches.h"
#include <algorithm>

using _CreateDXGIFactory = HRESULT (*)(REFIID riid, void **ppFactory);
using _CreateDXGIFactory2 = HRESULT (*)(UINT flags, REFIID riid,
                                        void **ppFactory);
using _DXGIGetDebugInterface1 = HRESULT (*)(UINT Flags, REFIID riid,
                                            void **pDebug);
using _DXGID3D10CreateDevice = HRESULT (*)(HMODULE hModule, void *pFactory,
                                           void *pAdapter, UINT Flags,
                                           void *pUnknown, void **ppDevice);
using _DXGID3D10CreateLayeredDevice = HRESULT (*)(void *pUnknown1,
                                                  void *pUnknown2,
                                                  void *pUnknown3,
                                                  void *pUnknown4,
                                                  void *pUnknown5);
using _DXGID3D10GetLayeredDeviceSize = SIZE_T (*)(const void *pLayers,
                                                  UINT NumLayers);
using _DXGID3D10RegisterLayers = HRESULT (*)(const void *pLayers,
                                             UINT NumLayers);
using _DirectInput8Create = HRESULT(__stdcall *)(HINSTANCE hinst,
                                                 DWORD dwVersion,
                                                 REFIID riidltf, LPVOID *ppvOut,
                                                 LPUNKNOWN punkOuter);
_CreateDXGIFactory CreateDXGIFactory_Original;
_CreateDXGIFactory CreateDXGIFactory1_Original;
_CreateDXGIFactory2 CreateDXGIFactory2_Original;
_DXGIGetDebugInterface1 DXGIGetDebugInterface1_Original;
_DXGID3D10CreateDevice DXGID3D10CreateDevice_Original;
_DXGID3D10CreateLayeredDevice DXGID3D10CreateLayeredDevice_Original;
_DXGID3D10GetLayeredDeviceSize DXGID3D10GetLayeredDeviceSize_Original;
_DXGID3D10RegisterLayers DXGID3D10RegisterLayers_Original;
_DirectInput8Create DirectInput8Create_Original;

HMODULE originalModule = nullptr;
HMODULE dllHandle = nullptr;

extern "C" {
__declspec(dllexport) HRESULT __stdcall CreateDXGIFactory(REFIID riid,
                                                          void **ppFactory) {
  consoleQueue.push("CreateDXGIFactory\n", gray);
  return CreateDXGIFactory_Original(riid, ppFactory);
}

__declspec(dllexport) HRESULT __stdcall CreateDXGIFactory1(REFIID riid,
                                                           void **ppFactory) {
  consoleQueue.push("CreateDXGIFactory1\n", gray);
  return CreateDXGIFactory1_Original(riid, ppFactory);
}

__declspec(dllexport) HRESULT __stdcall
CreateDXGIFactory2(UINT flags, REFIID riid, void **ppFactory) {
  consoleQueue.push("CreateDXGIFactory2\n", gray);
  return CreateDXGIFactory2_Original(flags, riid, ppFactory);
}

__declspec(dllexport) HRESULT __stdcall
DXGIGetDebugInterface1(UINT Flags, REFIID riid, void **pDebug) {
  consoleQueue.push("DXGIGetDebugInterface1\n", gray);
  return DXGIGetDebugInterface1_Original(Flags, riid, pDebug);
}

__declspec(dllexport) HRESULT __stdcall
DXGID3D10CreateDevice(HMODULE hModule, void *pFactory, void *pAdapter,
                      UINT Flags, void *pUnknown, void **ppDevice) {
  consoleQueue.push("DXGID3D10CreateDevice\n", gray);
  return DXGID3D10CreateDevice_Original(hModule, pFactory, pAdapter, Flags,
                                        pUnknown, ppDevice);
}

__declspec(dllexport) HRESULT __stdcall
DXGID3D10CreateLayeredDevice(void *pUnknown1, void *pUnknown2, void *pUnknown3,
                             void *pUnknown4, void *pUnknown5) {
  consoleQueue.push("DXGID3D10CreateLayeredDevice\n", gray);
  return DXGID3D10CreateLayeredDevice_Original(pUnknown1, pUnknown2, pUnknown3,
                                               pUnknown4, pUnknown5);
}

__declspec(dllexport) SIZE_T __stdcall
DXGID3D10GetLayeredDeviceSize(const void *pLayers, UINT NumLayers) {
  consoleQueue.push("DXGID3D10GetLayeredDeviceSize\n", gray);
  return DXGID3D10GetLayeredDeviceSize_Original(pLayers, NumLayers);
}

__declspec(dllexport) HRESULT __stdcall
DXGID3D10RegisterLayers(const void *pLayers, UINT NumLayers) {
  consoleQueue.push("DXGID3D10RegisterLayers\n", gray);
  return DXGID3D10RegisterLayers_Original(pLayers, NumLayers);
}

__declspec(dllexport) HRESULT __stdcall
DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
                   LPVOID *ppvOut, LPUNKNOWN punkOuter) {
  consoleQueue.push("DirectInput8Create\n", gray);
  return DirectInput8Create_Original(hinst, dwVersion, riidltf, ppvOut,
                                     punkOuter);
}
}

// Returns a string representation of the exception code for logging purposes
static std::string get_exception_name(const DWORD exception_code) {
  switch (exception_code) {
  case EXCEPTION_ACCESS_VIOLATION:
    return "EXCEPTION_ACCESS_VIOLATION";
  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
  case EXCEPTION_BREAKPOINT:
    return "EXCEPTION_BREAKPOINT";
  case EXCEPTION_DATATYPE_MISALIGNMENT:
    return "EXCEPTION_DATATYPE_MISALIGNMENT";
  case EXCEPTION_FLT_DENORMAL_OPERAND:
    return "EXCEPTION_FLT_DENORMAL_OPERAND";
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
  case EXCEPTION_FLT_INEXACT_RESULT:
    return "EXCEPTION_FLT_INEXACT_RESULT";
  case EXCEPTION_FLT_INVALID_OPERATION:
    return "EXCEPTION_FLT_INVALID_OPERATION";
  case EXCEPTION_FLT_OVERFLOW:
    return "EXCEPTION_FLT_OVERFLOW";
  case EXCEPTION_FLT_STACK_CHECK:
    return "EXCEPTION_FLT_STACK_CHECK";
  case EXCEPTION_FLT_UNDERFLOW:
    return "EXCEPTION_FLT_UNDERFLOW";
  case EXCEPTION_ILLEGAL_INSTRUCTION:
    return "EXCEPTION_ILLEGAL_INSTRUCTION";
  case EXCEPTION_IN_PAGE_ERROR:
    return "EXCEPTION_IN_PAGE_ERROR";
  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    return "EXCEPTION_INT_DIVIDE_BY_ZERO";
  case EXCEPTION_INT_OVERFLOW:
    return "EXCEPTION_INT_OVERFLOW";
  case EXCEPTION_INVALID_DISPOSITION:
    return "EXCEPTION_INVALID_DISPOSITION";
  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
  case EXCEPTION_PRIV_INSTRUCTION:
    return "EXCEPTION_PRIV_INSTRUCTION";
  case EXCEPTION_SINGLE_STEP:
    return "EXCEPTION_SINGLE_STEP";
  case EXCEPTION_STACK_OVERFLOW:
    return "EXCEPTION_STACK_OVERFLOW";
  case 0xE06D7363:
    return "CPP_EH_EXCEPTION";
  default:
    return "UNKNOWN_EXCEPTION";
  }
}

// Logs exceptions and steps over int3 breakpoints
static LONG WINAPI exception_logger(EXCEPTION_POINTERS *ExceptionInfo) {
  // Handle MSVC specific error codes
  if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406D1388) {
    // Extract the thread name from the exception record
    const auto threadName = reinterpret_cast<const char *>(
        ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
    const DWORD threadId = GetCurrentThreadId(); // Get the current thread ID
    consoleQueue.push("Thread name set: " + std::string(threadName) +
                          " (Thread ID: " + std::to_string(threadId) + ")\n",
                      gray);
    return EXCEPTION_CONTINUE_EXECUTION;
  }
  std::stringstream ss;
  ss << std::hex << std::uppercase
     << ExceptionInfo->ExceptionRecord->ExceptionCode;
  const std::string exceptionCodeHex = ss.str();
  consoleQueue.push(
      get_exception_name(ExceptionInfo->ExceptionRecord->ExceptionCode) +
          " (0x" + exceptionCodeHex + ")\n",
      red);
  if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
    // int3 makes a number of debug builds close on their own, so it's
    // automatically handled here for all titles.
#ifdef _AMD64_
    ExceptionInfo->ContextRecord->Rip++;
#else
    ExceptionInfo->ContextRecord->Eip++;
#endif
    return EXCEPTION_CONTINUE_EXECUTION;
  }
  if (ExceptionInfo->ExceptionRecord->ExceptionCode ==
      EXCEPTION_ACCESS_VIOLATION) {
    // Log the address that caused the access violation (can be useful for
    // debugging)
    const uintptr_t address =
        ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
    consoleQueue.push("Access violation at address: 0x" +
                          std::to_string(address) + "\n",
                      red);
    Sleep(5000);
  }
  return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: {
    dllHandle = hModule;

    char modulePath[MAX_PATH];
    GetModuleFileNameA(hModule, modulePath, MAX_PATH);
    std::string moduleName =
        std::filesystem::path(modulePath).filename().string();
    std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(),
                   ::tolower);

    if (moduleName == "dinput8.dll") {
      char path[MAX_PATH];
      GetSystemDirectoryA(path, MAX_PATH);
      strcat_s(path, "\\dinput8.dll");
      originalModule = LoadLibraryA(path);
      if (originalModule) {
        DirectInput8Create_Original = reinterpret_cast<_DirectInput8Create>(
            GetProcAddress(originalModule, "DirectInput8Create"));
      }
    } else {
      char path[MAX_PATH];
      GetSystemDirectoryA(path, MAX_PATH);
      strcat_s(path, "\\dxgi.dll");

      originalModule = LoadLibraryA(path);
      if (originalModule) {
        CreateDXGIFactory_Original = reinterpret_cast<_CreateDXGIFactory>(
            GetProcAddress(originalModule, "CreateDXGIFactory"));
        CreateDXGIFactory1_Original = reinterpret_cast<_CreateDXGIFactory>(
            GetProcAddress(originalModule, "CreateDXGIFactory1"));
        CreateDXGIFactory2_Original = reinterpret_cast<_CreateDXGIFactory2>(
            GetProcAddress(originalModule, "CreateDXGIFactory2"));
        DXGIGetDebugInterface1_Original =
            reinterpret_cast<_DXGIGetDebugInterface1>(
                GetProcAddress(originalModule, "DXGIGetDebugInterface1"));
        DXGID3D10CreateDevice_Original =
            reinterpret_cast<_DXGID3D10CreateDevice>(
                GetProcAddress(originalModule, "DXGID3D10CreateDevice"));
        DXGID3D10CreateLayeredDevice_Original =
            reinterpret_cast<_DXGID3D10CreateLayeredDevice>(
                GetProcAddress(originalModule, "DXGID3D10CreateLayeredDevice"));
        DXGID3D10GetLayeredDeviceSize_Original =
            reinterpret_cast<_DXGID3D10GetLayeredDeviceSize>(GetProcAddress(
                originalModule, "DXGID3D10GetLayeredDeviceSize"));
        DXGID3D10RegisterLayers_Original =
            reinterpret_cast<_DXGID3D10RegisterLayers>(
                GetProcAddress(originalModule, "DXGID3D10RegisterLayers"));
      }
    }

    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    consoleQueue.push("Kran's Debug DXGI Loaded\n", gray);

    init_hooks();
    AddVectoredExceptionHandler(1, exception_logger);

    consoleQueue.push("Exception handler added\n", green);
    apply_patches();

    break;
  }
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    FreeLibrary(originalModule);
    break;
  }

  return TRUE;
}
