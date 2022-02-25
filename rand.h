#pragma once

namespace mmh {
class GenRand {
public:
    void run(void* p_buffer, DWORD len)
    {
        memset(p_buffer, 0, len);
        CryptGenRandom(m_prov, len, static_cast<BYTE*>(p_buffer));
    }

    t_uint64 run_uint64()
    {
        t_uint64 ret;
        run(&ret, sizeof(ret));
        return ret;
    }

    t_uint64 run_uint64(t_uint64 min, t_uint64 max) { return min + (run_uint64() % (max - min)); }

    GenRand()
    {
        if (!CryptAcquireContext(&m_prov, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
            throw exception_win32(GetLastError());
    }

    ~GenRand()
    {
        CryptReleaseContext(m_prov, 0);
        m_prov = NULL;
    }

private:
    HCRYPTPROV m_prov;
};
}; // namespace mmh
