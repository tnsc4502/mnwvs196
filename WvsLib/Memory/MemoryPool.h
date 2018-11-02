/*-
* Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#pragma warning(disable:4624)  

#include <climits>
#include <cstddef>
#include <mutex>

template <typename T>
class MemoryPool
{
	std::mutex m_mtxLock;
public:
	/* Member types */
	typedef T               value_type;
	typedef T*              pointer;
	typedef T&              reference;
	typedef const T*        const_pointer;
	typedef const T&        const_reference;
	typedef size_t          size_type;
	typedef ptrdiff_t       difference_type;
	//   typedef std::false_type propagate_on_container_copy_assignment;
	//   typedef std::true_type  propagate_on_container_move_assignment;
	//   typedef std::true_type  propagate_on_container_swap;

	template <typename U> struct rebind {
		typedef MemoryPool<U> other;
	};

	size_t BlockSize;

	/* Member functions */
	MemoryPool(int nBlockSize) noexcept;
	MemoryPool(const MemoryPool& memoryPool) noexcept;
	MemoryPool(MemoryPool&& memoryPool) noexcept;
	template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept;

	~MemoryPool() noexcept;

	MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
	MemoryPool& operator=(MemoryPool&& memoryPool) noexcept;

	pointer address(reference x) const noexcept;
	const_pointer address(const_reference x) const noexcept;

	// Can only allocate one object at a time. n and hint are ignored
	pointer allocate(size_type n = 1, const_pointer hint = 0);
	void deallocate(pointer p, size_type n = 1);

	size_type max_size() const noexcept;

	template <class U, class... Args> void construct(U* p, Args&&... args);
	template <class U> void destroy(U* p);

	template <class... Args> pointer newElement(Args&&... args);
	template <class U, class... Args> pointer newElementCtor(Args&&... args);
	void deleteElement(pointer p);

private:
	union Slot_ {
		value_type element;
		Slot_* next;
	};

	typedef char* data_pointer_;
	typedef Slot_ slot_type_;
	typedef Slot_* slot_pointer_;

	slot_pointer_ currentBlock_;
	slot_pointer_ currentSlot_;
	slot_pointer_ lastSlot_;
	slot_pointer_ freeSlots_;

	size_type padPointer(data_pointer_ p, size_type align) const noexcept;
	void allocateBlock();

	//static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};


#include "MemoryPool.tcc"
#include <type_traits>

template<typename T, bool bLarge>
class WvsSingleObjectAllocator
{};

static MemoryPool<char[32]>  __gsgMemPool32 { 32 * 2048 };
static MemoryPool<char[64]>  __gsgMemPool64 { 64 * 1024 };
static MemoryPool<char[128]> __gsgMemPool128{ 128 * 512 };
static MemoryPool<char[256]> __gsgMemPool256{ 256 * 512 };
static MemoryPool<char[512]> __gsgMemPool512{ 512 * 512 };

template<typename T>
class WvsSingleObjectAllocator<T, false>
{
	std::mutex m_mtxLock;

public:
	inline void * ResourceMgr(bool allocate = true, int nSize = 1, void *pDel = nullptr)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		const int N = nSize;
		if (allocate)
		{
			if (N <= 32)
				return __gsgMemPool32.newElement();
			else if (N > 32 && N <= 64)
				return __gsgMemPool64.newElement();
			else if (N > 64 && N <= 128)
				return __gsgMemPool128.newElement();
			else if (N > 128 && N <= 256)
				return __gsgMemPool256.newElement();
			else if (N > 256 && N <= 512)
				return __gsgMemPool512.newElement();
		}
		else
		{
			if (N <= 32)
				__gsgMemPool32.deallocate((decltype(__gsgMemPool32)::pointer)pDel);
			else if (N > 32 && N <= 64)
				__gsgMemPool64.deallocate((decltype(__gsgMemPool64)::pointer)pDel);
			else if (N > 64 && N <= 128)
				__gsgMemPool128.deallocate((decltype(__gsgMemPool128)::pointer)pDel);
			else if (N > 128 && N <= 256)
				__gsgMemPool256.deallocate((decltype(__gsgMemPool256)::pointer)pDel);
			else if (N > 256 && N <= 512)
				__gsgMemPool512.deallocate((decltype(__gsgMemPool512)::pointer)pDel);
		}
		return nullptr;
	}

public:

	template<bool b>
	struct SELECTION {
		template<typename T>
		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			static_assert("Unexpected behavior.");
		}
	};

	template<>
	struct SELECTION<false> 
	{
		inline static void* alloc(WvsSingleObjectAllocator* t, int nSize)
		{
			return t->ResourceMgr(true, nSize);
		}

		inline static void free(WvsSingleObjectAllocator* t, T* res, int nSize)
		{
			t->ResourceMgr(false, nSize, res);
		}
	};

	template<>
	struct SELECTION<true> 
	{
		inline static void* alloc(WvsSingleObjectAllocator* t, int nSize)
		{
			T* res = (T*)t->ResourceMgr(true, nSize);
			//_ctor(res);
			new(res) T();
			return res;
		}

		inline static void free(WvsSingleObjectAllocator* t, T* res, int nSize)
		{
			res->~T();
			t->ResourceMgr(false, nSize, res);
		}
	};

	inline T * Allocate(int nSize)
	{
		return (T*)SELECTION<!std::is_array<T>::value>::alloc(this, nSize);
	}

	inline void Free(void *p, int nSize)
	{
		SELECTION<!std::is_array<T>::value>::free(this, reinterpret_cast<T*>(p), nSize);
	}

	inline static WvsSingleObjectAllocator<T, false>  * GetInstance() {
		static WvsSingleObjectAllocator<T, false> *p = new WvsSingleObjectAllocator<T, false>;
		return p;
	}

	template<class... Args>
	inline static T* AllocateWithCtor(Args&&... args)
	{
		const int N = sizeof(T);
		if (N <= 32)
			return (T*)__gsgMemPool32.newElementCtor<T>(std::forward<Args>(args)...);
		else if (N > 32 && N <= 64)
			return (T*)__gsgMemPool64.newElementCtor<T>(std::forward<Args>(args)...);
		else if (N > 64 && N <= 128)
			return (T*)__gsgMemPool128.newElementCtor<T>(std::forward<Args>(args)...);
		else if (N > 128 && N <= 256)
			return (T*)__gsgMemPool256.newElementCtor<T>(std::forward<Args>(args)...);
		else if (N > 256 && N <= 512)
			return (T*)__gsgMemPool512.newElementCtor<T>(std::forward<Args>(args)...);
	}
};

template<typename T>
class WvsSingleObjectAllocator<T, true>
{
	std::mutex m_mtxLock;
	MemoryPool<T> m_p;
public:	
	WvsSingleObjectAllocator()
		: m_p(sizeof(T) * 128)
	{
	}

	inline static WvsSingleObjectAllocator<T, true>  * GetInstance() {
		static WvsSingleObjectAllocator<T, true> *p = new WvsSingleObjectAllocator<T, true>;
		//p->m_p = new MemoryPool<T>(sizeof(T) * 128);
		return p;
	}

	inline T * Allocate(int nSize)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock); 
		T* res = m_p.newElement();
		//new(&res) T();
		return res;
	}


	template<bool b>
	struct SELECTION {
		template<typename T>
		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			static_assert("Unexpected behavior.");
		}
	};

	template<>
	struct SELECTION<false> {
		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			t->m_p.deallocate(res);
		}
	};

	template<>
	struct SELECTION<true> {
		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			res->~T();
			t->m_p.deallocate(res);
		}
	};

	inline void Free(void *p, int nSize)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		SELECTION<!std::is_array<T>::value>::free(this, reinterpret_cast<T*>(p));
	}

	template<class... Args>
	inline static T* AllocateWithCtor(Args&&... args)
	{
		return (T*)GetInstance()->m_p.newElementCtor<T>(std::forward<Args>(args)...);
	}
	/*void Free(void *p)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		m_p->deleteElement(p);
	}*/
};

class WvsArrayAllocator
{
	std::mutex m_mtxLock;

		static MemoryPool<char[32]> __gMemPool32  ;
		static MemoryPool<char[64]> __gMemPool64  ;
		static MemoryPool<char[128]> __gMemPool128;
		static MemoryPool<char[256]> __gMemPool256;
		static MemoryPool<char[512]> __gMemPool512;
private:
	template<typename T>
	inline void * ResourceMgr(bool allocate = true, int nSize = 1, void *pDel = nullptr)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		const int N = (int)nSize;
		if (allocate)
		{
			if (N <= 32)
				return __gMemPool32.newElement();
			else if (N > 32 && N <= 64)
				return __gMemPool64.newElement();
			else if (N > 64 && N <= 128)
				return __gMemPool128.newElement();
			else if (N > 128 && N <= 256)
				return __gMemPool256.newElement();
			else if (N > 256 && N <= 512)
				return __gMemPool512.newElement();
			else
				return new T[nSize];
		}
		else
		{
			if (N <= 32)
				__gMemPool32.deallocate((decltype(__gMemPool32)::pointer)pDel);
			else if (N > 32 && N <= 64)
				__gMemPool64.deallocate((decltype(__gMemPool64)::pointer)pDel);
			else if (N > 64 && N <= 128)
				__gMemPool128.deallocate((decltype(__gMemPool128)::pointer)pDel);
			else if (N > 128 && N <= 256)
				__gMemPool256.deallocate((decltype(__gMemPool256)::pointer)pDel);
			else if (N > 256 && N <= 512)
				__gMemPool512.deallocate((decltype(__gMemPool512)::pointer)pDel);
			else
				delete[](T*)pDel;
		}
		return nullptr;
	}

public:

	template<typename T>
	inline void * Allocate(int nSize)
	{
		return ResourceMgr<T>(true, nSize);
	}

	template<typename T>
	inline void Free(void *p, int nSize)
	{
		ResourceMgr<T>(false, nSize, p);
	}

	inline static WvsArrayAllocator * GetInstance() {
		static WvsArrayAllocator *p = new WvsArrayAllocator;
		return p;
	}
};

#pragma warning(default:4624)  
#endif // MEMORY_POOL_H
