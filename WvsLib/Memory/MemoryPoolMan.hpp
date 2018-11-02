#pragma once

#include "Arena.h"
#include "boost\pool\pool.hpp"
#include "boost\pool\singleton_pool.hpp"
#include "MemoryPool.h"
/*
簡單記憶體配置池，用於管理讀取Wz時的記憶體消耗
@By Wanger.
*/

/*
[+]
for object allocation
*/

//#define AllocObj(instance_type) new instance_type()
//#define FreeObj(pointer) delete pointer
#define AllocObj(instance_type) WvsSingleObjectAllocator<instance_type, (sizeof(instance_type) > 512)>::GetInstance()->Allocate(sizeof(instance_type))
#define AllocObjCtor(instance_type) WvsSingleObjectAllocator<instance_type, (sizeof(instance_type) > 512)>::AllocateWithCtor
#define FreeObj(pointer) WvsSingleObjectAllocator<std::remove_reference<decltype(*pointer)>::type, (sizeof(*pointer) > 512)>::GetInstance()->Free(pointer, (sizeof(*pointer)))
#define FreeObj_T(T, pointer) WvsSingleObjectAllocator<T, (sizeof(T) > 512)>::GetInstance()->Free(pointer, sizeof(T))

#define AllocArray(instance_type, nSize) (instance_type*)WvsArrayAllocator::GetInstance()->Allocate<char>(nSize * sizeof(instance_type))
#define FreeArray(pointer, nSize) WvsArrayAllocator::GetInstance()->Free<char>(pointer, nSize * sizeof(*pointer))

class WzMemoryPoolMan
{
private:

	memt::Arena *pArena = new memt::Arena();
public:
	static WzMemoryPoolMan* GetInstance() 
	{
		static WzMemoryPoolMan* pInstance = new WzMemoryPoolMan;
		return pInstance;
	}

	WzMemoryPoolMan() {};

	/*static MemoryPoolMan *GetInstance()
	{
		static MemoryPoolMan *sMemoryPool = new MemoryPoolMan();
		return sMemoryPool;
	}*/

	//配置一個指標物件，大小為size (byte)
	void* AllocateObject(int size)
	{
		return pArena->alloc(size);
	}

#pragma warning(disable:4312)  
#pragma warning(disable:4311)  
#pragma warning(disable:4302) 
	//配置一個陣列，長度為len bytes
	void* AllocateArray(int len)
	{
		//return singleton_char_pool::ordered_malloc(len);

		auto allocPair = pArena->allocArray<char>(len + 4);
 
		//強制把end寫在前四個bytes
		*((int*)allocPair.first) = (int)allocPair.second;
		return allocPair.first + 4;
	}
#pragma warning(default:4302)  
#pragma warning(disable:4311)  
#pragma warning(disable:4312)  

	//將給定的 ptr (一個物件)  銷毀
	void DestructObject(void* ptr)
	{
		pArena->freeTop(ptr);
	}

	//將給定的 ptr (一個陣列) 銷毀，其中陣列的終端位置記錄在給定指標的前四個byte
	void DestructArray(void* ptr)
	{
		//singleton_char_pool::ordered_free(ptr);
		//g_ptrMemPool->FreeMemory((char*)ptr - 4, (int)(*(int*)(((char*)ptr) - 4)));
		pArena->freeTopArray(((char*)((char*)ptr) - 4), (char*)(*(int*)(((char*)ptr) - 4)));
	}

	//釋放記憶池，將空間歸還給OS
	void Release()
	{
		pArena->freeAllAllocsAndBackingMemory();
	}
};