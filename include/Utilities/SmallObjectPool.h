#pragma  once 
#include <memory.h>

template<class T,int PRE_ALLOCATE_OBJECT_NUM,int MAX_OBJECT_NUM>
class CSmallObjectPool
{
public:
	CSmallObjectPool():m_iNextAvailabelIdx(0) 
	{  
		m_pool = new T[PRE_ALLOCATE_OBJECT_NUM];
		m_iCapacity = PRE_ALLOCATE_OBJECT_NUM;
	}

	T& operator[](int idx)             { return m_pool[idx];}
	const T& operator[](int idx) const { return m_pool[idx];}
	void Clear() {Recycle();}
	void Recycle()                     
	{ 
	 /*   if(m_iCapacity > MAX_OBJECT_NUM)
		{
			delete []m_pool;
			m_pool = new T[PRE_ALLOCATE_OBJECT_NUM];
		}*/
		m_iNextAvailabelIdx = 0;
	}
	size_t size () { return m_iNextAvailabelIdx;}
	T      Get()   { return m_pool[GetNextAvailable(1)];}
	
protected:
	inline int GetNextAvailable(int nObjNum)
	{
		int nNextIndex = m_iNextAvailabelIdx+nObjNum;
		if(nNextIndex >= m_iCapacity)
		{
			T* pool = new T[nNextIndex+PRE_ALLOCATE_OBJECT_NUM];
			memcpy(pool,m_pool,sizeof(T)*m_iNextAvailabelIdx);
			delete []m_pool;
			m_pool = pool;
			m_iCapacity  += (nNextIndex+PRE_ALLOCATE_OBJECT_NUM);
		}
		int ret = m_iNextAvailabelIdx ;
		m_iNextAvailabelIdx =nNextIndex;
		return ret;
	}

protected:
	int m_iNextAvailabelIdx;
	T   *m_pool;
	int m_iCapacity;


};

template<class T,int PRE_ALLOCATE_OBJECT_NUM>
class CStaticSmallObjectPool
{
public:

	CStaticSmallObjectPool():m_iNextAvailabelIdx(0) 
	{  
		m_pool = new T[PRE_ALLOCATE_OBJECT_NUM];
	}

	T& operator[](int idx)             { return m_pool[idx];}
	const T& operator[](int idx) const { return m_pool[idx];}
	void Clear()                       { Recycle();}
	void Recycle()                     { m_iNextAvailabelIdx = 0;}
	int GetNextAvailable(int nObjNum)
	{
		int ret = m_iNextAvailabelIdx ;
		m_iNextAvailabelIdx += nObjNum;
		return ret;
	}
	size_t size () { return m_iNextAvailabelIdx;}
	T      Get()   { return m_pool[GetNextAvailable(1)];}


protected:
	int m_iNextAvailabelIdx;
	T   *m_pool;
};