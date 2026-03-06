#pragma once
#include "console.h"
#include "functions.h"
#include "hooks.h"
#include "PatchLoader.h"


// Replacement for GetSystemTimeAsFileTime to return a fixed date, used to bypass license checks in Dishonored 2
VOID WINAPI GetSystemTimeAsFileTimeHook(LPFILETIME lpSystemTimeAsFileTime)
{
    SYSTEMTIME st;
    st.wYear = 2018;
    st.wMonth = 1;
    st.wDay = 1;
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    SystemTimeToFileTime(&st, lpSystemTimeAsFileTime);
}

// Detects the game and version to apply the appropriate patches
void apply_patches()
{
    const auto game = get_pdb_path();
    // get product name instead of PDB path (for games that don't have PDBs linked)
    const auto fallbackName = get_product_name();
    const auto version = get_exe_version();
    const auto timestamp = get_pe_timestamp();

    const auto time = static_cast<time_t>(timestamp);

    // Use std::localtime_s for thread safety
    tm timeInfo;
    if (localtime_s(&timeInfo, &time) != 0)
    {
        return;
    }

    // Format the timestamp into a human-readable string
    std::ostringstream formatted_timestamp;
    formatted_timestamp << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

    consoleQueue.push("Detected game: " + fallbackName + " (" + game + ") v" + version + " Timestamp: " + formatted_timestamp.str() + '\n', gray);

    // Load and apply byte patches from external files
    PatchLoader::ApplyPatches();

    // Game specific hooks (that cannot be simple byte patches)
    if (game == "Dishonored2_x64")
    {
        create_hook(GetSystemTimeAsFileTime, GetSystemTimeAsFileTimeHook);
        consoleQueue.push("Patched license check (Dishonored 2)\n", green);
    }
}
