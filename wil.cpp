#include "stdafx.h"

namespace mmh {

std::string get_caught_exception_message() noexcept
{
    try {
        throw;
    } catch (const wil::ResultException& result_exception) {
        std::array<wchar_t, 2048> message;
        (void)wil::GetFailureLogString(message.data(), message.size(), result_exception.GetFailureInfo());
        return to_utf8(message.data());
    } catch (const std::exception& exception) {
        return exception.what();
    }
}

} // namespace mmh
