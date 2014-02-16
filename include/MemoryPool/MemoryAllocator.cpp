#include "MemoryAllocator.h"
#include <memory.h>

Mem_Allocator::Obj * MEM_VOLATILE Mem_Allocator::m_sfree_list[NFREELIST]=
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
char * Mem_Allocator::m_start_free = 0;
char * Mem_Allocator::m_end_free = 0;
size_t Mem_Allocator::m_heapsize = 0;

#ifdef MTTHREADS
CLock Mem_Allocator::m_lock;
#endif 

