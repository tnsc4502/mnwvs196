#include "MemoryPool.h"

MemoryPool<char[32]>  WvsArrayAllocator::__gMemPool32{ 32 * 2048 };
MemoryPool<char[64]>  WvsArrayAllocator::__gMemPool64{ 64 * 1024 };
MemoryPool<char[128]> WvsArrayAllocator::__gMemPool128{ 128 * 512 };
MemoryPool<char[256]> WvsArrayAllocator::__gMemPool256{ 256 * 512 };
MemoryPool<char[512]> WvsArrayAllocator::__gMemPool512{ 512 * 512 };