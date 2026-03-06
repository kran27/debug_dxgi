#pragma once

// Tests an address for validity before writing a block of data to it.
inline void Write(uintptr_t address, const std::vector<uint8_t>& data)
{
    if (IsBadReadPtr(reinterpret_cast<void*>(address), data.size())) return;
    if (IsBadWritePtr((LPVOID)address, data.size())) return;
    std::memcpy(reinterpret_cast<void*>(address), data.data(), data.size());
}

// Helper function to apply a byte patch at a specific offset in the game executable and log the result.
inline bool apply_byte_patch(const uintptr_t offset, const std::vector<uint8_t>& data, std::string patch_name)
{
    DWORD old_protect;
    if (auto addr = reinterpret_cast<LPVOID>(GameExe + offset); VirtualProtect(addr, data.size(), PAGE_EXECUTE_READWRITE, &old_protect))
    {
        Write(reinterpret_cast<uintptr_t>(addr), data);
        VirtualProtect(addr, data.size(), old_protect, &old_protect);
        consoleQueue.push("Patched " + patch_name + '\n', green);
        return true;
    }
    consoleQueue.push("Failed to patch " + patch_name + '\n', red);
    return false;
}

// Retrieves the PDB path from the current module. Used for game identification as it's not affected by file renaming.
inline std::string get_pdb_path()
{
    for (const auto pe = Pe::PeNative::fromModule(GetModuleHandleW(nullptr)); const auto& debug : pe.debug())
    {
        const std::string pdbPath = pe.byRva<Pe::CodeView::DebugInfo>(debug.debugEntry()->AddressOfRawData)->pdb70.pdbName;
        std::string pdbName = pdbPath.substr(pdbPath.find_last_of('\\') + 1);
        pdbName = pdbName.substr(0, pdbName.find_last_of('.'));
        return pdbName;
    }
    return "";
}

// Retrieves the name of the executable file associated with the current process.
inline std::string get_exe_name()
{
    if (const HMODULE h_module = GetModuleHandle(nullptr); h_module != nullptr)
    {
        if (CHAR sz_path[MAX_PATH]; GetModuleFileNameA(h_module, sz_path, MAX_PATH) != 0)
        {
            std::string exe_name(sz_path);
            if (const size_t pos = exe_name.find_last_of("\\/"); pos != std::string::npos)
            {
                exe_name = exe_name.substr(pos + 1);
            }
            return exe_name;
        }
    }
    return "";
}

// Retrieves the version of the executable file associated with the current process.
// Absolute last resort for game identification, as it is affected by file renaming. Not currently used.
//
// TODO: use Pe.hpp to get version info
inline std::string get_exe_version()
{
    const HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        CHAR szPath[MAX_PATH];
        if (GetModuleFileNameA(hModule, szPath, MAX_PATH) != 0)
        {
            DWORD dwDummy;
            const DWORD dwSize = GetFileVersionInfoSizeA(szPath, &dwDummy);
            if (dwSize != 0)
            {
                const auto lpData = new BYTE[dwSize];
                if (lpData != nullptr)
                {
                    if (GetFileVersionInfoA(szPath, 0, dwSize, lpData) != 0)
                    {
                        UINT uLen;
                        VS_FIXEDFILEINFO* pFileInfo;
                        if (VerQueryValueA(lpData, "\\", reinterpret_cast<LPVOID*>(&pFileInfo), &uLen) != 0)
                        {
                            const WORD majorVersion = HIWORD(pFileInfo->dwFileVersionMS);
                            const WORD minorVersion = LOWORD(pFileInfo->dwFileVersionMS);
                            const WORD buildNumber = HIWORD(pFileInfo->dwFileVersionLS);
                            const WORD revisionNumber = LOWORD(pFileInfo->dwFileVersionLS);

                            char version[256];
                            sprintf_s(version, "%d.%d.%d.%d", majorVersion, minorVersion, buildNumber, revisionNumber);
                            delete[] lpData;
                            return version;
                        }
                    }
                    delete[] lpData;
                }
            }
        }
    }
    return "";
}

// Retrieves the timestamp from the PE header of the current process, for identifying extra-stubborn executables.
inline DWORD get_pe_timestamp()
{
    const auto pe = Pe::PeNative::fromModule(GetModuleHandleW(nullptr));
    if (const auto nt = pe.headers().nt(); nt != nullptr)
    {
        return nt->FileHeader.TimeDateStamp;
    }
    return 0;
}

// Retrieves the product name from the executable file associated with the current process.
// Used for game identification in cases where there's no associated PDB file.
// Not primary method as debug builds often share product name with other builds.
inline std::string get_product_name()
{
    char szPath[MAX_PATH];
    if (GetModuleFileNameA(nullptr, szPath, MAX_PATH) == 0)
        return "";

    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeA(szPath, &dummy);
    if (size == 0)
        return "";

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoA(szPath, 0, size, data.data()))
        return "";

    // Get the translation table to find the correct language and code page
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    UINT cbTranslate = 0;
    if (!VerQueryValueA(data.data(), "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) || cbTranslate < sizeof(LANGANDCODEPAGE))
        return "";

    // Format the query string using the language and code page
    char subBlock[128];
    sprintf_s(subBlock, "\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate->wLanguage, lpTranslate->wCodePage);

    // Retrieve the ProductName
    LPSTR lpProductName = nullptr;
    UINT sizeProductName = 0;
    if (VerQueryValueA(data.data(), subBlock, (LPVOID*)&lpProductName, &sizeProductName) && lpProductName)
    {
        return std::string(lpProductName);
    }

    return "";
}
