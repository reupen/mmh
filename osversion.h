#pragma once

namespace mmh {

bool check_windows_10_build(DWORD build_number);
bool is_windows_10_or_newer();
bool is_windows_8_or_newer();
bool is_windows_7_or_newer();
bool is_windows_vista_or_newer();
bool is_wine();

} // namespace mmh
