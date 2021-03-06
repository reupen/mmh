#include "stdafx.h"

namespace mmh {
bool is_windows_7_or_newer()
{
    static bool ret = IsWindows7OrGreater();
    return ret;
}
bool is_windows_vista_or_newer()
{
    static bool ret = IsWindowsVistaOrGreater();
    return ret;
}
bool is_windows_xp_or_newer()
{
    static bool ret = IsWindowsXPOrGreater();
    return ret;
}
bool test_osversion(WORD major, WORD minor, WORD sp)
{
    return IsWindowsVersionOrGreater(major, minor, sp);
}
} // namespace mmh
