#pragma once
#include <windows.h>
#include <cstdint>
#include <ShlObj.h>
#include <list>
#include <filesystem>
#include <queue>
#include <condition_variable>
#include <iostream>
#pragma comment(lib, "Version.lib")

#include "console.h"
#include "Pe.hpp"
#include "minhook-master/include/MinHook.h"
#ifdef NDEBUG
#pragma comment(lib, "lib/Release/libMinHook.x64.lib")
#else
#pragma comment(lib, "lib/Debug/libMinHook.x64.lib")
#endif

static auto GameExe = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
inline console_queue consoleQueue = console_queue();
