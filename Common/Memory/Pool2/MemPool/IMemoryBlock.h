/******************
IMemoryBlock.h
******************/

/*!\file IMemoryBlock.h
 * \brief Contains the "IMemoryBlock" Class-defintion.
 *        This is the (abstract) interface for the actual MemoryPool-Class.
 */


#ifndef __INC_IMemoryBlock_h__
#define __INC_IMemoryBlock_h__

#include "../BasicIncludes.h"

/*!\namespace MemPool
 * \brief MemoryPool Namespace
 *
 * This Namespace contains all classes and typedefs needed by
 * the MemoryPool implementation.
 * The MemoryPool has its own namespace because some typedefs
 * (e.g. TByte) may intefer with other toolkits if the
 * MemoryPool would be in the global namespace.
 */
namespace MemPool
{

/*!\typedef unsigned char TByte ;
 * \brief Byte (= 8 Bit) Typedefinition.
 */
typedef unsigned char TByte ;

/*!\class IMemoryBlock
 * \brief Interface Class (pure Virtual) for the MemoryPool
 *
 * Abstract Base-Class (interface) for the MemoryPool.
 * Introduces Basic Operations like Geting/Freeing Memory.
 */
class IMemoryBlock
{
  public :
    virtual ~IMemoryBlock() {} ;

    virtual void *GetMemory(const std::size_t &sMemorySize) = 0 ;
    virtual void FreeMemory(void *ptrMemoryBlock, const std::size_t &sMemoryBlockSize) = 0 ;
} ;

}
#endif /* __INC_IMemoryBlock_h__ */
