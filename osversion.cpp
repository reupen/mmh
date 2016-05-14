#include "stdafx.h"

namespace mmh
{
	namespace osversion
	{
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
		bool test_osversion(DWORD major, DWORD minor, DWORD sp)
		{
			return IsWindowsVersionOrGreater(major, minor, sp);
		}
	};

}