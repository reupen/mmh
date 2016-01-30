#pragma once

namespace mmh {
	class thread_t {
	public:
		void create_thread()
		{
			if (!m_thread)
				m_thread = CreateThread(NULL, NULL, &g_threadproc, (LPVOID)this, NULL, NULL);
		}
		void on_destroy_thread()
		{
			if (m_thread)
			{
				WaitForSingleObject(m_thread, 10 * 1000);
				CloseHandle(m_thread);
				m_thread = NULL;
			}
		}
		HANDLE get_thread() { return m_thread; }
		void release_thread()
		{
			if (m_thread)
			{
				CloseHandle(m_thread);
				m_thread = NULL;
			}
		}
		virtual DWORD on_thread() = 0;
		thread_t() : m_thread(NULL) {};

		virtual ~thread_t() {};
	private:
		static DWORD CALLBACK g_threadproc(LPVOID lpThreadParameter)
		{
			thread_t * p_this = reinterpret_cast<thread_t*>(lpThreadParameter);
			return p_this->on_thread();
		}
		HANDLE m_thread;
	};

	class thread_v2_t {
	public:
		void create_thread()
		{
			if (!m_thread)
				m_thread = CreateThread(NULL, NULL, &g_on_thread, (LPVOID)this, NULL, NULL);
		}
		void set_priority(int priority)
		{
			m_priority = priority;
			if (m_thread != NULL)
				SetThreadPriority(m_thread, m_priority);
		}
		bool wait_for_thread(DWORD timeout = pfc_infinite)
		{
			if (m_thread)
			{
				return WAIT_TIMEOUT != WaitForSingleObject(m_thread, timeout);
			}
			return true;
		}
		void wait_for_and_release_thread(DWORD timeout = pfc_infinite)
		{
			wait_for_thread(timeout);
			release_thread();
		}
		bool is_thread_open() { return m_thread != NULL; }
		HANDLE get_thread() { return m_thread; }
		virtual DWORD on_thread() = 0;
		thread_v2_t() : m_thread(NULL), m_priority(THREAD_PRIORITY_NORMAL) {};

		virtual ~thread_v2_t() {};

		void release_thread()
		{
			if (m_thread)
			{
				CloseHandle(m_thread);
				m_thread = NULL;
			}
		}

	private:
		static DWORD CALLBACK g_on_thread(LPVOID lpThreadParameter)
		{
			thread_v2_t * p_this = reinterpret_cast<thread_v2_t*>(lpThreadParameter);
			if (p_this->m_priority)
				SetThreadPriority(GetCurrentThread(), p_this->m_priority);
			return p_this->on_thread();
		}
		HANDLE m_thread;
		int m_priority;
	};

}