#pragma once

namespace mmh
{
	namespace osversion
	{
		bool is_windows_7_or_newer();
		bool is_windows_vista_or_newer();
		bool is_windows_xp_or_newer();
		bool test_osversion(DWORD major, DWORD minor, DWORD sp = 0);
	};

}