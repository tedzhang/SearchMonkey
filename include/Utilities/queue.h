#pragma  once 

template<class T>
class CQueue 
{
public :
	CQueue(int nMaxCount);
	~CQueue();
	bool IsEmpty()const;
	bool EnQueue(const T& element);
	bool DeQueue(T& element);
private:
	T*  m_pQue;
	int m_nHead;
	int m_nTail;
	int m_nMaxCount;
};

template<class T>
inline CQueue<T>::CQueue(int nMaxCount)
:m_nHead(0)
,m_nTail(0)
,m_nMaxCount(nMaxCount+1)
{
  m_pQue = new T[m_nMaxCount];
}

template<class T>
inline bool CQueue<T>::EnQueue(const T& element)
{
	int nNextPos = (m_nTail+1)%m_nMaxCount;
	if(nNextPos != m_nHead)
	{
		m_pQue[m_nTail] = element;
		m_nTail         = nNextPos;
		return true;
	}
	//queue is full,tell caller
	return false;

}

template<class T>
inline bool CQueue<T>::DeQueue( T& element)
{
   if(IsEmpty()) return false;
   element = m_pQue[m_nHead];
   m_nHead = (m_nHead+1)%(m_nMaxCount);
   return true;
}

template<class T>
inline bool CQueue<T>::IsEmpty() const
{
	return m_nHead == m_nTail;
}

template<class T>
inline CQueue<T>::~CQueue()
{
	delete []m_pQue;
}