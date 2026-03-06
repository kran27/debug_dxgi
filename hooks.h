#pragma once
#include "console.h"

// Hook to redirect OutputDebugStringA calls to the created console window
void OutputDebugStringAHook(LPCSTR lpOutputString)
{
    consoleQueue.push(lpOutputString, white);
}

// Converts a wide string (LPCWSTR) to a UTF-8 encoded std::string
inline std::string convert_lpwstr_to_string(LPCWSTR lpwstrInput)
{
    const int nChars = WideCharToMultiByte(CP_UTF8, 0, lpwstrInput, -1, nullptr, 0, nullptr, nullptr);
    std::string strOutput(nChars, 0);
    WideCharToMultiByte(CP_UTF8, 0, lpwstrInput, -1, &strOutput[0], nChars, nullptr, nullptr);
    return strOutput;
}

// Hook to redirect OutputDebugStringW calls to the created console window
void OutputDebugStringWHook(LPCWSTR lpOutputString)
{
    const std::string strOutput = convert_lpwstr_to_string(lpOutputString);
    consoleQueue.push(strOutput, white);
}

// Helper function to create and enable a hook using MinHook
inline void create_hook(LPVOID fpFunction, LPVOID lpCallback)
{
    MH_CreateHook(fpFunction, lpCallback, nullptr);
    MH_EnableHook(fpFunction);
}

// This function initializes the hooks for OutputDebugStringA and OutputDebugStringW for logging purposes.
// Automatically handles games which use either kernel32.dll or kernelbase.dll for debug output.
inline void init_hooks()
{
    // previously api-ms-win-core-debug-l1-1-0.dll, this should be more universal
    // idk if a game can use both kernel32.dll and kernelbase.dll
    // if it has both but uses kernel32.dll for debug output, it will hook the wrong function
    HMODULE hModule = GetModuleHandle(L"kernelbase.dll");
    if (hModule == nullptr)
    {
        hModule = GetModuleHandle(L"kernel32.dll");
    }
    MH_Initialize();
    const FARPROC fpOutputDebugStringA = GetProcAddress(hModule, "OutputDebugStringA");
    create_hook(fpOutputDebugStringA, OutputDebugStringAHook);
    const FARPROC fpOutputDebugStringW = GetProcAddress(hModule, "OutputDebugStringW");
    create_hook(fpOutputDebugStringW, OutputDebugStringWHook);
    
    consoleQueue.push("Debug Output Hooked\n", green);
}
