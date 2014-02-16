#pragma  once 
//
//
// Provide a memory pool implementation for new and delete operation
//
#include "MemoryAllocator.h"

class CBaseAlloc
{
public:
	virtual ~CBaseAlloc(){ }

	static void *operator new(size_t size )
	{
		return Mem_Allocator::allocate(size);
	}
	static void operator delete(void *p,size_t size)
	{
		 Mem_Allocator::deallocate(p,size);
	}
	static void* operator new[](size_t size )
	{
        return Mem_Allocator::allocate(size);
	};
	static void operator delete[](void *p,size_t size )
	{
		Mem_Allocator::deallocate(p,size);
	}
};