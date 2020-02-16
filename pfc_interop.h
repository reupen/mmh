#pragma once

namespace mmh {

namespace _ {

template <typename T>
using size = decltype(std::declval<T>().size());

}

template <class Container>
auto get_container_size(Container&& container)
{
    if constexpr (is_detected_v<_::size, Container>) {
        return container.size();
    } else {
        return container.get_size();
    }
}

} // namespace mmh
