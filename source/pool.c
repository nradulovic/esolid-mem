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
 * @author      Nenad Radulovic
 * @brief       Pool Memory management Implementation
 * @addtogroup  mem_pool
 *********************************************************************//** @{ */
/**@defgroup    mem_pool_impl Implementation
 * @brief       Pool Memory management Implementation
 * @{ *//*--------------------------------------------------------------------*/

/*=========================================================  INCLUDE FILES  ==*/

#include "plat/critical.h"
#include "base/bitop.h"
#include "mem/pool.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for pool memory manager
 */
#define POOL_MEM_SIGNATURE              ((esAtomic)0xdeadbeeeu)

/*======================================================  LOCAL DATA TYPES  ==*/

/**@brief       Pool allocator header structure
 */
struct poolMemBlock {
    struct poolMemBlock * next; /**<@brief Next free block                                  */
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

/**@brief       Module information
 */
static const ES_MODULE_INFO_CREATE("PoolMem", "Pool Memory management", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

esError esPoolMemInit(
    struct esPoolMem * poolMem,
    void * array,
    size_t arraySize,
    size_t blockSize) {

    size_t blockCnt;
    size_t nBlocks;
    struct poolMemBlock * block;

    blockSize = ES_ALIGN_UP(blockSize, sizeof(esAtomic));

    ES_REQUIRE(ES_API_POINTER, poolMem != NULL);
    ES_REQUIRE(ES_API_POINTER, array != NULL);
    ES_REQUIRE(ES_API_RANGE,   blockSize != 0u);
    ES_REQUIRE(ES_API_RANGE,   blockSize <= arraySize);

    nBlocks = arraySize / blockSize;
    poolMem->size = arraySize;
    poolMem->free = arraySize;
    poolMem->blockSize = blockSize;
    poolMem->sentinel = (struct poolMemBlock *) array;
    block = poolMem->sentinel;

    for (blockCnt = 0u; blockCnt < nBlocks - 1u; blockCnt++) {
        block->next = (struct poolMemBlock *) ((uint8_t *) block
                        + poolMem->blockSize);
        block = block->next;
    }
    block->next = NULL;

    ES_OBLIGATION(
        poolMem->signature = POOL_MEM_SIGNATURE);

    return (ES_ERROR_NONE);
}

esError esPoolMemAllocI(
    struct esPoolMem *  poolMem,
    size_t              size,
    void **             mem) {

    ES_REQUIRE(ES_API_POINTER, poolMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  poolMem->signature == POOL_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, mem != NULL);

    if (size > poolMem->blockSize) {

        return (ES_ERROR_ARG_OUT_OF_RANGE);
    } else if (poolMem->sentinel == NULL) {

        return (ES_ERROR_NO_MEMORY);
    } else {
        struct poolMemBlock * block;

        block = poolMem->sentinel;
        poolMem->sentinel = block->next;
        poolMem->free -= poolMem->blockSize;
        *mem = (void *)block;

        return (ES_ERROR_NONE);
    }
}

esError esPoolMemAlloc(
    struct esPoolMem *  poolMem,
    size_t              size,
    void **             mem) {

    esError             error;
    esIntrCtx           intrCtx;

    ES_CRITICAL_LOCK_ENTER(&intrCtx);
    error = esPoolMemAllocI(
        poolMem,
        size,
        mem);
    ES_CRITICAL_LOCK_EXIT(intrCtx);

    return (error);
}

esError esPoolMemFreeI(
    struct esPoolMem *  poolMem,
    void *              mem) {

    struct poolMemBlock * block;

    ES_REQUIRE(ES_API_POINTER, poolMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  poolMem->signature == POOL_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, mem != NULL);

    block = (struct poolMemBlock *)mem;
    block->next = poolMem->sentinel;
    poolMem->sentinel = block;
    poolMem->free += poolMem->blockSize;

    return (ES_ERROR_NONE);
}

esError esPoolMemFree(
    struct esPoolMem *  poolMem,
    void *              mem) {

    esError             error;
    esIntrCtx           intrCtx;

    ES_CRITICAL_LOCK_ENTER(&intrCtx);
    error = esPoolMemFreeI(
        poolMem,
        mem);
    ES_CRITICAL_LOCK_EXIT(intrCtx);

    return (error);
}

esError esPoolMemGetFreeI(
    struct esPoolMem *  poolMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, poolMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  poolMem->signature == POOL_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, size != NULL);

    *size = poolMem->free;

    return (ES_ERROR_NONE);
}

esError esPoolMemGetBlockSizeI(
    struct esPoolMem *  poolMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, poolMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  poolMem->signature == POOL_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, size != NULL);

    *size = poolMem->blockSize;

    return (ES_ERROR_NONE);
}

esError esPoolMemGetBlockSize(
    struct esPoolMem *  poolMem,
    size_t *            size) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esPoolMemGetBlockSizeI(poolMem, size);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of pool_mem.c
 ******************************************************************************/
