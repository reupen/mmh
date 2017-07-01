#include "stdafx.h"

namespace mmh 
{
    int g_compare_context(void* context, const void * item1, const void* item2)
    {
        return ((sort_base*)(context))->compare(item1, item2);
    }
}
