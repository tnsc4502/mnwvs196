#pragma once

#include "Arena.h"
#include "boost\pool\singleton_pool.hpp"
/*
簡單記憶體配置池，用於管理讀取Wz時的記憶體消耗
@By Wanger.
*/

class WzMemoryPoolMan
{
private:
	struct char_pool {};
	typedef boost::singleton_pool<char_pool, sizeof(char)> singleton_char_pool;

	memt::Arena *pArena = new memt::Arena();
public:
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

//Used for WZ.
extern WzMemoryPoolMan *stWzMemoryPoolMan;

class MSMemoryPoolMan
{
private:
	struct char_pool {};
	typedef boost::singleton_pool<char_pool, sizeof(char)> singleton_char_pool;
	memt::Arena *pArena = new memt::Arena();

public:
	MSMemoryPoolMan() {};

#pragma warning(disable:4312)  
#pragma warning(disable:4311)  
#pragma warning(disable:4302) 
	//配置一個陣列，長度為len bytes
	void* AllocateArray(int len)
	{
		return singleton_char_pool::ordered_malloc(len);
	}

	void* AllocateObject(int size)
	{
		return pArena->alloc(size);
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
		singleton_char_pool::ordered_free(ptr);
	}

	//釋放記憶池，將空間歸還給OS
	void Release()
	{
		singleton_char_pool::release_memory();
	}
};

//Memory Allocator for regular use.
extern MSMemoryPoolMan *stMemoryPoolMan;

struct ArenaUniquePtrDeleter
{
	void operator()(unsigned char* ptr)
	{
		stMemoryPoolMan->DestructArray(ptr);
		//delete[] ptr;
	}
};