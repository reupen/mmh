#pragma once

namespace mmh {
class IStreamMemblock : public IStream {
    long refcount;
    pfc::array_t<t_uint8> m_data;
    t_size m_position;

public:
    // const void * get_ptr() {return m_data;} const;

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject)
    {
        if (ppvObject == nullptr)
            return E_NOINTERFACE;
        if (iid == IID_IUnknown) {
            AddRef();
            *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if (iid == IID_IStream) {
            AddRef();
            *ppvObject = static_cast<IStream*>(this);
            return S_OK;
        }
        if (iid == IID_ISequentialStream) {
            AddRef();
            *ppvObject = static_cast<ISequentialStream*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef() { return InterlockedIncrement(&refcount); }

    virtual ULONG STDMETHODCALLTYPE Release()
    {
        LONG rv = InterlockedDecrement(&refcount);
        if (!rv)
            delete this;
        return rv;
    }

    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
    {
        if (dwOrigin == STREAM_SEEK_CUR) {
            if (dlibMove.QuadPart + (LONGLONG)m_position < 0
                || dlibMove.QuadPart + (LONGLONG)m_position > (LONGLONG)m_data.get_size())
                return STG_E_INVALIDFUNCTION;
            m_position += (t_size)dlibMove.QuadPart; // Cast should be OK by if condition
        } else if (dwOrigin == STREAM_SEEK_END) {
            if (dlibMove.QuadPart + (LONGLONG)m_data.get_size() < 0)
                return STG_E_INVALIDFUNCTION;
            m_position = m_data.get_size() - (t_size)dlibMove.QuadPart; // Cast should be OK by if condition
        } else if (dwOrigin == STREAM_SEEK_SET) {
            if ((ULONGLONG)dlibMove.QuadPart > m_data.get_size())
                return STG_E_INVALIDFUNCTION;
            m_position = (t_size)dlibMove.QuadPart; // Cast should be OK by if condition
        } else
            return STG_E_INVALIDFUNCTION;
        if (plibNewPosition)
            plibNewPosition->QuadPart = m_position;
        return S_OK;
    }

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
        /* [length_is][size_is][out] */ void* pv, ULONG cb,
        /* [out] */ ULONG* pcbRead)
    {
        t_size read = (std::min)(static_cast<unsigned>(cb), m_data.get_size() - m_position);
        memcpy(pv, &m_data.get_ptr()[m_position], read);
        m_position += read;
        if (pcbRead)
            *pcbRead = read;
        return S_OK;
    }

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
        /* [size_is][in] */ const void* pv, ULONG cb,
        /* [out] */ ULONG* pcbWritten)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE CopyTo(
        /* [unique][in] */ IStream* pstm, ULARGE_INTEGER cb,
        /* [out] */ ULARGE_INTEGER* pcbRead,
        /* [out] */ ULARGE_INTEGER* pcbWritten)
    {
        if (cb.QuadPart > m_data.get_size() - m_position)
            return STG_E_INVALIDFUNCTION;
        t_size read = (std::min)((t_size)cb.QuadPart, m_data.get_size() - m_position);
        ULONG pwritten = NULL;
        pstm->Write(&m_data.get_ptr()[m_position], read, &pwritten);
        m_position += read;
        if (pcbRead)
            pcbRead->QuadPart = read;
        if (pcbWritten)
            pcbWritten->QuadPart = pwritten;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE Revert() { return E_FAIL; }

    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Stat(
        /* [out] */ __RPC__out STATSTG* pstatstg, DWORD grfStatFlag)
    {
        memset(pstatstg, 0, sizeof(STATSTG));
        pstatstg->cbSize.QuadPart = m_data.get_size();
        pstatstg->type = STGTY_STREAM;
        pstatstg->pwcsName = nullptr;
        /*if (!(grfStatFlag & STATFLAG_NONAME))
        {
            if (pstatstg->pwcsName = (LPOLESTR)CoTaskMemAlloc(1*2))
                wcscpy_s(pstatstg->pwcsName, 5, L"AB.jpg");
        }*/
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ __RPC__deref_out_opt IStream** ppstm)
    {
        *ppstm = new IStreamMemblock(m_data.get_ptr(), m_data.get_size());
        return S_OK;
    }

    IStreamMemblock(const t_uint8* p_data, t_size size) : refcount(0), m_position(0)
    {
        m_data.append_fromptr(p_data, size);
    }
};
} // namespace mmh
