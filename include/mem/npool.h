/*
 * This file is part of eSolid.
 *
 * Copyright (C) 2010 - 2013 Nenad Radulovic
 *
 * eSolid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eSolid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with eSolid.  If not, see <http://www.gnu.org/licenses/>.
 *
 * web site:    http://github.com/nradulovic
 * e-mail  :    nenad.b.radulovic@gmail.com
 *//***********************************************************************//**
 * @file
 * @author  	Nenad Radulovic
 * @brief       Pool Memory Management module header
 * @defgroup    mem_pool Pool Memory management
 * @brief       Pool Memory management
 *********************************************************************//** @{ */
/**@defgroup    mem_pool_intf Interface
 * @brief       Pool memory API
 * @{ *//*--------------------------------------------------------------------*/

#ifndef ES_POOL_MEM_H_
#define ES_POOL_MEM_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "base/debug.h"
#include "base/error.h"

/*===============================================================  MACRO's  ==*/

#define ES_POOL_MEM_COMPUTE_SIZE(blocks, blockSize)                             \
    ((blocks) * (ES_ALIGN_UP(blockSize, sizeof(esAtomic))))

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Pool memory instance
 * @details     This structure holds information about pool memory instance.
 * @p           This structure hold information about pool and block sizes.
 *              Additionally, it holds a guard member which will ensure mutual
 *              exclusion in preemption environments.
 * @see         esPoolMemInit()
 * @api
 */
struct esPoolMem {
    struct poolMemBlock * sentinel;                                             /**<@brief Pointer to the pool sentinel                     */
    size_t              free;
    size_t              size;                                                   /**<@brief The size of pool memory                          */
    size_t              blockSize;                                              /**<@brief Size of one block                                */
#if (CONFIG_API_VALIDATION == 1) || defined(__DOXYGEN__)
    esAtomic            signature;                                              /**<@brief Structure signature, used during development only*/
#endif
};

/**@brief       Pool memory instance poolMem type
 * @api
 */
typedef struct esPoolMem esPoolMem;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/**@brief       Initializes pool memory instance
 * @param       poolMem
 *              Pointer to pool memory instance, see @ref esPoolMem.
 * @param       pool
 *              Reserved memory area for pool allocator.
 * @param       poolSize
 *              The size of reserved memory area expressed in bytes.
 * @param       blockSize
 *              The size of one block expressed in bytes.
 * @return      Error status
 *              ES_ERROR_NONE
 * @details     This function must be called before any call to esPoolMemAllocI()
 *              or esPoolMemAlloc().
 * @warning     Pointers to @c poolMem and @c pool must be aligned to CPU defined
 *              alignment.
 * @api
 */
esError esPoolMemInit(
    struct esPoolMem *  poolMem,
    void *              array,
    size_t              arraySize,
    size_t              blockSize);

/**@brief       Calculates required reserved memory size for defined number of
 *              blocks.
 * @param       blocks
 *              Number of required blocks.
 * @param       blockSize
 *              The size of one block.
 * @return      Required reserved memory size.
 * @api
 */
size_t esPoolMemComputeSize(
    size_t              blocks,
    size_t              blockSize);

/**@brief       Allocate one block from memory pool
 * @param       poolMem
 *              Pointer to pool memory instance, see @ref esPoolMem.
 * @param       size
 *              The size of requested memory. If you know pool size then you can
 *              safely put '0' here.
 * @param       mem
 *              Pointer to requested block.
 * @return      eSolid standard error:
 *              - @ref ES_ERROR_NONE - no error occurred
 *              - @ref ES_ERROR_NO_MEMORY - not enough memory available
 *              - @ref ES_ERROR_ARG_OUT_OF_RANGE - requested size is bigger than
 *                  the pool block size
 * @iclass
 */
esError esPoolMemAllocI(
    struct esPoolMem *  poolMem,
    size_t              size,
    void **             mem);

/**@brief       Allocate one block from memory pool
 * @param       poolMem
 *              Pointer to pool memory instance, see @ref esPoolMem.
 * @param       size
 *              The size of requested memory. If you know pool size then you can
 *              safely put '0' here.
 * @param       mem
 *              Pointer to requested block.
 * @return      eSolid standard error:
 *              - @ref ES_ERROR_NONE - no error occurred
 *              - @ref ES_ERROR_NO_MEMORY - not enough memory available
 *              - @ref ES_ERROR_ARG_OUT_OF_RANGE - requested size is bigger than
 *                  the pool block size
 * @api
 */
esError esPoolMemAlloc(
    struct esPoolMem *  poolMem,
    size_t              size,
    void **             mem);

/**
 * @brief       Oslobadja prethodno alocirani blok
 * @param       [in] poolMem             Deskriptor pool alokatora
 * @param       [in] mem                Prethodno alociran blok memorije
 * @iclass
 */
esError esPoolMemFreeI(
    struct esPoolMem *  poolMem,
    void *              mem);

/**
 * @brief       Oslobadja prethodno alocirani blok
 * @param       [in] poolMem             Deskriptor pool alokatora
 * @param       [in] mem                Prethodno alociran blok memorije
 * @note        Funkcija koristi makroe @ref ES_CRITICAL_LOCK_ENTER i
 *              @ref ES_CRITICAL_LOCK_EXIT za zastitu memorije od istovremenog
 *              pristupa.
 * @api
 */
esError esPoolMemFree(
    struct esPoolMem *  poolMem,
    void *              mem);

esError esPoolMemGetFreeI(
    struct esPoolMem *  poolMem,
    size_t *            size);

esError esPoolMemGetBlockSizeI(
    struct esPoolMem *  poolMem,
    size_t *            size);

esError esPoolMemGetBlockSize(
    struct esPoolMem *  poolMem,
    size_t *            size);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of pool_mem.h
 ******************************************************************************/
#endif /* ES_POOL_MEM_H_ */
