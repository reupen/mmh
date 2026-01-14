#include "stdafx.h"

namespace mmh {

bool check_windows_10_build(DWORD build_number)
{
    OSVERSIONINFOEX osviex{};
    osviex.dwOSVersionInfoSize = sizeof(osviex);

    DWORDLONG mask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
    mask = VerSetConditionMask(mask, VER_BUILDNUMBER, VER_GREATER_EQUAL);

    osviex.dwMajorVersion = 10;
    osviex.dwMinorVersion = 0;
    osviex.dwBuildNumber = build_number;

    return VerifyVersionInfoW(&osviex, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, mask) != FALSE;
}

bool is_windows_10_or_newer()
{
    static auto result = IsWindows10OrGreater();
    return result;
}

bool is_windows_8_or_newer()
{
    static bool result = IsWindows8OrGreater();
    return result;
}

bool is_windows_7_or_newer()
{
    static bool result = IsWindows7OrGreater();
    return result;
}

bool is_windows_vista_or_newer()
{
    static bool result = IsWindowsVistaOrGreater();
    return result;
}

bool is_wine()
{
    static bool result = [] {
        wil::unique_hmodule ntdll_module(LoadLibrary(L"ntdll.dll"));

        if (!ntdll_module.is_valid())
            return false;

        return GetProcAddress(ntdll_module.get(), "wine_get_version") != nullptr;
    }();

    return result;
}

} // namespace mmh
