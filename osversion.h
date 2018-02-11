#pragma once

namespace mmh {
bool is_windows_7_or_newer();
bool is_windows_vista_or_newer();
bool is_windows_xp_or_newer();
bool test_osversion(WORD major, WORD minor, WORD sp = 0);
} // namespace mmh
