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

#ifndef MEMORY_BLOCK_TCC
#define MEMORY_BLOCK_TCC

template <typename T>
inline typename MemoryPool<T>::size_type
MemoryPool<T>::padPointer(data_pointer_ p, size_type align)
const noexcept
{
  uintptr_t result = reinterpret_cast<uintptr_t>(p);
  return ((align - result) % align);
}

template <typename T>
MemoryPool<T>::MemoryPool(int nBlockSize) noexcept
{
  BlockSize = (size_t)pow(2, ceil(log2(nBlockSize)) + 1);
  currentBlock_ = nullptr;
  currentSlot_ = nullptr;
  lastSlot_ = nullptr;
  freeSlots_ = nullptr;
}

template <typename T>
MemoryPool<T>::MemoryPool(const MemoryPool& memoryPool)
noexcept :
MemoryPool()
{}

template <typename T>
MemoryPool<T>::MemoryPool(MemoryPool&& memoryPool)
noexcept
{
  currentBlock_ = memoryPool.currentBlock_;
  memoryPool.currentBlock_ = nullptr;
  currentSlot_ = memoryPool.currentSlot_;
  lastSlot_ = memoryPool.lastSlot_;
  freeSlots_ = memoryPool.freeSlots;
}

template <typename T>
template<class U>
MemoryPool<T>::MemoryPool(const MemoryPool<U>& memoryPool)
noexcept :
MemoryPool()
{}

template <typename T>
MemoryPool<T>&
MemoryPool<T>::operator=(MemoryPool&& memoryPool)
noexcept
{
  if (this != &memoryPool)
  {
    std::swap(currentBlock_, memoryPool.currentBlock_);
    currentSlot_ = memoryPool.currentSlot_;
    lastSlot_ = memoryPool.lastSlot_;
    freeSlots_ = memoryPool.freeSlots;
  }
  return *this;
}

template <typename T>
MemoryPool<T>::~MemoryPool()
noexcept
{
  slot_pointer_ curr = currentBlock_;
  while (curr != nullptr) {
    slot_pointer_ prev = curr->next;
    operator delete(reinterpret_cast<void*>(curr));
    curr = prev;
  }
}

template <typename T>
inline typename MemoryPool<T>::pointer
MemoryPool<T>::address(reference x)
const noexcept
{
  return &x;
}

template <typename T>
inline typename MemoryPool<T>::const_pointer
MemoryPool<T>::address(const_reference x)
const noexcept
{
  return &x;
}

template <typename T>
void
MemoryPool<T>::allocateBlock()
{
  // Allocate space for the new block and store a pointer to the previous one
  data_pointer_ newBlock = reinterpret_cast<data_pointer_>
                           (operator new(BlockSize));
  reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
  currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
  // Pad block body to staisfy the alignment requirements for elements
  data_pointer_ body = newBlock + sizeof(slot_pointer_);
  size_type bodyPadding = padPointer(body, alignof(slot_type_));
  currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
  lastSlot_ = reinterpret_cast<slot_pointer_>
              (newBlock + BlockSize - sizeof(slot_type_) + 1);
}

template <typename T>
inline typename MemoryPool<T>::pointer
MemoryPool<T>::allocate(size_type n, const_pointer hint)
{
  //std::lock_guard<std::mutex> lock_(m_mtxLock);
  if (freeSlots_ != nullptr) {
    pointer result = reinterpret_cast<pointer>(freeSlots_);
    freeSlots_ = freeSlots_->next;
    return result;
  }
  else {
    if (currentSlot_ >= lastSlot_)
      allocateBlock();
    return reinterpret_cast<pointer>(currentSlot_++);
  }
}

template <typename T>
inline void
MemoryPool<T>::deallocate(pointer p, size_type n)
{
  //std::lock_guard<std::mutex> lock_(m_mtxLock);
  if (p != nullptr) {
    reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
    freeSlots_ = reinterpret_cast<slot_pointer_>(p);
  }
}

template <typename T>
inline typename MemoryPool<T>::size_type
MemoryPool<T>::max_size()
const noexcept
{
  size_type maxBlocks = -1 / BlockSize;
  return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

template <typename T>
template <class U, class... Args>
inline void
MemoryPool<T>::construct(U* p, Args&&... args)
{
  new (p) U (std::forward<Args>(args)...);
}

template <typename T>
template <class U>
inline void
MemoryPool<T>::destroy(U* p)
{
  p->~U();
}

template <typename T>
template <class... Args>
inline typename MemoryPool<T>::pointer
MemoryPool<T>::newElement(Args&&... args)
{
  pointer result = allocate();
  construct<value_type>(result, std::forward<Args>(args)...);
  return result;
}

template <typename T>
template <typename U, class... Args>
inline typename MemoryPool<T>::pointer
MemoryPool<T>::newElementCtor(Args&&... args)
{
  pointer result = allocate();
  construct<U>((U*)result, std::forward<Args>(args)...);
  return result;
}

template <typename T>
inline void
MemoryPool<T>::deleteElement(pointer p)
{
  if (p != nullptr) {
    p->~value_type();
    deallocate(p);
  }
}

#pragma warning(default:4624)  
#endif // MEMORY_BLOCK_TCC
