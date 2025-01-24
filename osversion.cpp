#include "stdafx.h"

namespace mmh {

bool is_windows_10_or_newer()
{
    static auto result = IsWindows10OrGreater();
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

} // namespace mmh
