#pragma  once 
//
// The allocator class is adopted from SGI STL library
// 2006/12/10 Yong Zhang(yongzhang81@gmail.com)
// if you use in multi-thread environment ,you need to
// define the macro MTTHREADS
//
#include <memory.h>
#include <stdlib.h>

#define xmalloc(s) malloc((s))
#define xfree(p)   free((p))
#define xrealloc(p,newsz) realloc(p,newsz)

#ifdef MTTHREADS
#define MEM_VOLATILE volatile
#else
#define MEM_VOLATILE
#endif

#ifdef MTTHREADS
#include "../Utilities/Lock.h"
#endif

class Mem_Allocator
{
public:
	enum{ALIGN = 8};
	enum{MAX_BYTES=128};
	enum{NFREELIST = MAX_BYTES/ALIGN};

	static void  *allocate(size_t n);
	static void  deallocate(void *ptr,size_t n);
	static void *reallocate(void *p,size_t oldsz,size_t newsz);
private:
	
	static size_t roundup(size_t  bytes)
	{
		return (bytes +((size_t)ALIGN-1)) &(~((size_t)ALIGN -1));
	}
	union Obj
	{
		union Obj * m_free_list_link;
		char m_clientdata[1];
	};
	static Obj * MEM_VOLATILE m_sfree_list[NFREELIST];

	static size_t  s_freelist_index(size_t bytes)
	{
		return (((bytes) + (size_t)ALIGN-1)/(size_t)ALIGN - 1);
	}

	static void* s_refill(size_t n);
	static char* chunk_alloc(size_t size, int &nobjs);

private:
	static char * m_start_free;
	static char * m_end_free;
	static size_t m_heapsize;
#ifdef MTTHREADS

	static CLock  m_lock;

	class LockWrapper;
	friend class LockWrapper;
	class LockWrapper
	{
	 public:
		 LockWrapper()
		 {
			 m_lock.acquire_lock();
		 }
		 ~LockWrapper()
		 {
			 m_lock.release_lock();
		 }

	};
#endif 

};


//
//implementation

inline void *Mem_Allocator::allocate(size_t n)
{
	void *ret = 0;
	if(n > MAX_BYTES)
	{
		ret = xmalloc(n);
	}
	else
	{
		Obj *MEM_VOLATILE *my_free_list = m_sfree_list +s_freelist_index(n);
#ifdef MTTHREADS
		LockWrapper lock;
#endif 
		Obj *result = *my_free_list;
		if(result == 0)
		{
			ret = s_refill(roundup(n));
		}
		else
		{
			*my_free_list = result->m_free_list_link;
			ret  = result;
		}

	}
	return ret;
}

inline void Mem_Allocator::deallocate(void *ptr,size_t n)
{
	if(n > size_t(MAX_BYTES))
	{
		xfree(ptr);
	}
	else
	{
		Obj *MEM_VOLATILE *my_free_list = m_sfree_list +s_freelist_index(n);
		Obj *q = (Obj *)ptr;
#ifdef MTTHREADS
		LockWrapper lock;
#endif
		q->m_free_list_link = *my_free_list;
		*my_free_list       = q;
	}
}


inline char *Mem_Allocator::chunk_alloc(size_t size, int &nobjs)
{
	char *result;
	size_t total_bytes = size *nobjs;
	size_t bytes_left  = m_end_free - m_start_free;
	if(bytes_left >= total_bytes)
	{
		result = m_start_free;
		m_start_free += total_bytes;
		return result;
	}
	else if(bytes_left >= size)
	{
		nobjs = (int)(bytes_left/size);
		total_bytes =size *nobjs;
		result = m_start_free;
		m_start_free += total_bytes;
		return result;
	}
	else
	{
		size_t bytes_to_get = 2 *total_bytes + roundup(m_heapsize >>4);
		if(bytes_left > 0)
		{
			Obj *MEM_VOLATILE *my_free_list =
				m_sfree_list + s_freelist_index(bytes_left);
			((Obj*)m_start_free)->m_free_list_link = *my_free_list;
			*my_free_list = (Obj*)m_start_free;

		}
		m_start_free =(char*)xmalloc(bytes_to_get);
		if( 0== m_start_free)
		{
			size_t i;
			Obj* MEM_VOLATILE *my_free_list;
			Obj* p;

			for( i = size; i <= (size_t)MAX_BYTES;i+=(size_t)ALIGN)
			{
				my_free_list = m_sfree_list +s_freelist_index(i);
				p = *my_free_list;
				if( p)
				{
					*my_free_list = p->m_free_list_link;
					m_start_free = (char*)p;
					m_end_free   = m_start_free+i;
					return chunk_alloc(size,nobjs);
				}
			}
			m_end_free = 0;
			m_start_free = (char*)xmalloc(bytes_to_get);

		}
		m_heapsize  += bytes_to_get;
		m_end_free = m_start_free + bytes_to_get;
		return chunk_alloc(size,nobjs);

	}

}

inline void* Mem_Allocator::s_refill(size_t n)
{
	int nObjs = 20;
	Obj* MEM_VOLATILE *my_free_list;
	Obj* result;
	Obj *next_obj;
	Obj *current_obj;

	char *chunk = chunk_alloc(n,nObjs);
	if(1 == nObjs) return chunk;
	my_free_list = m_sfree_list+s_freelist_index(n);
	result =(Obj*)chunk;

	*my_free_list =next_obj =(Obj*)(chunk+n);
	for(int i = 1; ;i++)
	{
		current_obj = next_obj;
		next_obj =(Obj*)((char*)next_obj+n);
		if(nObjs -1 == i)
		{
			current_obj->m_free_list_link = 0;
			break;
		}
		else
		{
			current_obj->m_free_list_link = next_obj;
		}
	}
	return result;
}

inline void* Mem_Allocator::reallocate(void *p,size_t oldsz,size_t newsz)
{
	void *result;
	size_t copy_sz;
	if(oldsz > (size_t)MAX_BYTES || newsz >(size_t)MAX_BYTES)
	{
		return xrealloc(p,newsz);
	}

	if(roundup(oldsz) == roundup(newsz)) return p;

	result = allocate(newsz);
	copy_sz = newsz > oldsz? oldsz:newsz;
	memcpy(result,p,copy_sz);
	deallocate(p,oldsz);
	return result;
}