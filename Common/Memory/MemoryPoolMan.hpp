#pragma once

#include "Arena.h"

/*
簡單記憶體配置池，用於管理讀取Wz時的記憶體消耗
@By Wanger.
*/

class MemoryPoolMan
{
private:
	memt::Arena *pArena = new memt::Arena();

public:
	MemoryPoolMan() {};

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

	//配置一個陣列，長度為len bytes
	void* AllocateArray(int len)
	{
		auto allocPair = pArena->allocArray<char>(len + 4);
#pragma warning(disable:4312)  
#pragma warning(disable:4311)  
#pragma warning(disable:4302)  
		//強制把end寫在前四個bytes
		*((int*)allocPair.first) = (int)allocPair.second;
		return allocPair.first + 4;
#pragma warning(default:4302)  
#pragma warning(disable:4311)  
#pragma warning(disable:4312)  
	}

	//將給定的 ptr (一個物件)  銷毀
	void DestructObject(void* ptr)
	{
		pArena->freeTop(ptr);
	}

	//將給定的 ptr (一個陣列) 銷毀，其中陣列的終端位置記錄在給定指標的前四個byte
	void DestructArray(void* ptr)
	{
		pArena->freeTopArray(((char*)((char*)ptr) - 4), (char*)(*(int*)(((char*)ptr) - 4)));
	}

	//釋放記憶池，將空間歸還給OS
	void Release()
	{
		pArena->freeAllAllocsAndBackingMemory();
	}
};

//Memory Allocator for regular use.
extern MemoryPoolMan *stMemoryPoolMan;

//Used for WZ.
extern MemoryPoolMan *stWzMemoryPoolMan;

struct ArenaUniquePtrDeleter
{
	void operator()(unsigned char* ptr)
	{
		stMemoryPoolMan->DestructArray(ptr);
	}
};