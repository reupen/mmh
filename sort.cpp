#include "stdafx.h"

namespace mmh {

std::vector<size_t> create_permutation(size_t size)
{
    std::vector<size_t> permutation(size);
    fill_identity_permutation(permutation);
    return permutation;
}

void fill_identity_permutation(std::span<size_t> permutation)
{
    std::iota(permutation.begin(), permutation.end(), size_t{});
}

} // namespace mmh
