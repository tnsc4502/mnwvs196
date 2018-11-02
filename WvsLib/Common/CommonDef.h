#pragma once
#define ALLOW_PRIVATE_ALLOC template<class T> friend class MemoryPool; template<class T, bool b> friend class WvsSingleObjectAllocator;