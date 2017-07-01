#pragma once

namespace mmh {
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;

    /**
    * \brief Gets the HINSTANCE corresponding to the current module.
    *
    * See https://blogs.msdn.microsoft.com/oldnewthing/20041025-00/?p=37483.
    *
    * \return HINSTANCE of current module
    */
    inline HINSTANCE get_current_instance()
    {
        return reinterpret_cast<HINSTANCE>(&__ImageBase);
    }
}
