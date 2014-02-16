
#pragma  once 

#include<windows.h>
class CLock
{
public:
	CLock()
	{
		InitializeCriticalSection(&m_critical);
	}
	void acquire_lock()
	{
		EnterCriticalSection(&m_critical);
	}
	void release_lock()
	{
		LeaveCriticalSection(&m_critical);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_critical);
	}
private:
	CRITICAL_SECTION m_critical;


};