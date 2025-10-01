#include "stdafx.h"

using namespace std::string_view_literals;

namespace mmh::win32 {

std::wstring format_error(DWORD error_code)
{
    constexpr auto buffer_size = 256u;

    std::wstring message;
    message.resize(buffer_size);

    (void)FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message.data(), buffer_size, nullptr);

    message.resize(wcslen(message.data()));

    const auto last_non_newline_char = message.find_last_not_of(L"\r\n ."sv);

    if (last_non_newline_char != std::wstring::npos)
        message.resize(last_non_newline_char + 1);
    else
        message.clear();

    if (message.empty())
        message = L"Unknown error"sv;

    std::format_to(std::back_inserter(message), L" (0x{:08x})", error_code);

    return message;
}

} // namespace mmh::win32
