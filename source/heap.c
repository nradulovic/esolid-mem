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
 * @brief       Heap Memory Management Implementation
 * @addtogroup  mem_heap
 *********************************************************************//** @{ */
/**@defgroup    mem_heap_impl Implementation
 * @brief       Heap Memory Management Implementation
 * @{ *//*--------------------------------------------------------------------*/

/*=========================================================  INCLUDE FILES  ==*/

#include "plat/critical.h"
#include "family/profile.h"
#include "base/bitop.h"
#include "mem/heap.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for dynamic memory manager
 */
#define HEAP_MEM_SIGNATURE              ((esAtomic)0xdeadbeefu)

/*======================================================  LOCAL DATA TYPES  ==*/

/**@brief       Dynamic allocator memory block header structure
 */
struct PORT_C_ALIGN(ES_CPU_DEF_DATA_ALIGNMENT) heapMemBlock {
    struct heapPhy {
        struct heapMemBlock * prev;
        esRamSSize            size;
    }                   phy;
    struct heapFree {
        struct heapMemBlock * next;
        struct heapMemBlock * prev;
    }                   free;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

/**@brief       Module information
 */
static const ES_MODULE_INFO_CREATE("HeapMem", "Heap Memory management", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

esError esHeapMemInit(
    struct esHeapMem *  heapMem,
    void *              storage,
    size_t              storageSize) {

    struct heapMemBlock * begin;

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature != HEAP_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, storage != NULL);
    ES_REQUIRE(ES_API_RANGE,   storageSize > sizeof(struct heapMemBlock [2]));
    ES_REQUIRE(ES_API_RANGE,   storageSize < ES_RAM_SSIZE_MAX);

    storageSize = ES_ALIGN(storageSize, ES_CPU_DEF_DATA_ALIGNMENT);
    heapMem->sentinel =
        (struct heapMemBlock *)((uint8_t *)storage + storageSize) - 1;          /* Sentinel is the last element of the storage              */
    begin = (struct heapMemBlock *)storage;
    begin->phy.size  = (esRamSSize)(storageSize
        - sizeof(struct heapMemBlock [1]) - sizeof(struct heapPhy [1]));
    begin->phy.prev  = heapMem->sentinel;
    begin->free.next = heapMem->sentinel;
    begin->free.prev = heapMem->sentinel;

    heapMem->sentinel->phy.size  = -1;
    heapMem->sentinel->phy.prev  = begin;
    heapMem->sentinel->free.next = begin;
    heapMem->sentinel->free.prev = begin;
    heapMem->size = (size_t)begin->phy.size;
    heapMem->free = heapMem->size;

    ES_OBLIGATION(heapMem->signature = HEAP_MEM_SIGNATURE);

    return (ES_ERROR_NONE);
}

esError esHeapMemTerm(
    struct esHeapMem *  heapMem) {

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature == HEAP_MEM_SIGNATURE);

    heapMem->sentinel = NULL;

    ES_OBLIGATION(heapMem->signature = ~HEAP_MEM_SIGNATURE);

    return (ES_ERROR_NONE);
}

esError esHeapMemAllocI(
    struct esHeapMem *  heapMem,
    size_t              size,
    void **             mem) {

    struct heapMemBlock * curr;

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature == HEAP_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_RANGE,   (size != 0u) && (size < ES_RAM_SSIZE_MAX));
    ES_REQUIRE(ES_API_POINTER, mem != NULL);

    size = ES_ALIGN_UP(size, sizeof(struct heapPhy [1]));
    curr = heapMem->sentinel->free.next;

    while (curr != heapMem->sentinel) {

        if ((size_t)curr->phy.size >= size) {

            if ((size_t)curr->phy.size >
                (size + sizeof(struct heapMemBlock [1]))) {
                struct heapMemBlock * tmp;

                *mem = (void *)(&curr->free);
                tmp = (struct heapMemBlock *)
                    ((uint8_t *)curr + size + sizeof(struct heapPhy [1]));      /* Create smaller free block                                */
                tmp->phy.prev  = curr;                                          /* Point back to the current block                          */
                tmp->phy.size  =
                    curr->phy.size - (esRamSSize)size - (esRamSSize)sizeof(struct heapPhy [1]);
                curr->phy.size = (esRamSSize)size * (-1);                       /* Mark block as allocated                                  */
                tmp->free.next = curr->free.next;                               /* Remove current and add smaller free block to free list   */
                tmp->free.prev = curr->free.prev;
                tmp->free.next->free.prev = tmp;
                tmp->free.prev->free.next = tmp;
                curr           = (struct heapMemBlock *)
                    ((uint8_t *)tmp + tmp->phy.size + sizeof(struct heapPhy [1]));
                curr->phy.prev = tmp;                                           /* Point to the newly created block                         */

                return (ES_ERROR_NONE);
            } else {
                *mem = (void *)(&curr->free);
                curr->free.next->free.prev = curr->free.prev;                   /* Remove current block from free list                      */
                curr->free.prev->free.next = curr->free.next;
                curr->phy.size = (esRamSSize)(curr->phy.size * (-1));           /* Mark block as allocated                                  */

                return (ES_ERROR_NONE);
            }
        }
        curr = curr->free.next;
    }

    return (ES_ERROR_NO_MEMORY);
}

esError esHeapMemAlloc(
    struct esHeapMem *  heapMem,
    size_t              size,
    void **             mem) {

    esError             error;
    esAtomic            intCtx;

    ES_CRITICAL_LOCK_ENTER(&intCtx);
    error = esHeapMemAllocI(
        heapMem,
        size,
        mem);
    ES_CRITICAL_LOCK_EXIT(intCtx);

    return (error);
}

esError esHeapMemFreeI(
    struct esHeapMem *  heapMem,
    void *              mem) {

    struct heapMemBlock * curr;
    struct heapMemBlock * tmp;

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature == HEAP_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, mem != NULL);

    curr           = (struct heapMemBlock *)
        ((uint8_t *)mem - offsetof(struct heapMemBlock, free));
    curr->phy.size = (esRamSSize)curr->phy.size * (-1);                         /* Mark block as free                                       */
    tmp            = (struct heapMemBlock *)
        ((uint8_t *)curr + curr->phy.size + sizeof(struct heapPhy [1]));

    if ((curr->phy.prev->phy.size > 0) && (tmp->phy.size < 0)) {               /* Previous block is free                                   */
        curr->phy.prev->phy.size +=
            curr->phy.size + (esRamSSize)sizeof(struct heapPhy [1]);
        tmp->phy.prev             = curr->phy.prev;
    } else if ((curr->phy.prev->phy.size < 0) && (tmp->phy.size > 0)) {        /* Next block is free                                       */
        curr->free.next            = tmp->free.next;
        curr->free.prev            = tmp->free.prev;
        curr->free.prev->free.next = curr;
        curr->free.next->free.prev = curr;
        curr->phy.size += tmp->phy.size + (esRamSSize)sizeof(struct heapPhy [1]);
        tmp = (struct heapMemBlock *)
            ((uint8_t *)curr + curr->phy.size + sizeof(struct heapPhy [1]));
        tmp->phy.prev   = curr;
    } else if ((curr->phy.prev->phy.size > 0) && (tmp->phy.size > 0)) {       /* Previous and next blocks are free                        */
        tmp->free.prev->free.next = tmp->free.next;
        tmp->free.next->free.prev = tmp->free.prev;
        curr->phy.prev->phy.size +=
            curr->phy.size + tmp->phy.size + (esRamSSize)sizeof(struct heapPhy [2]);
        tmp = (struct heapMemBlock *)
            ((uint8_t *)curr->phy.prev + curr->phy.prev->phy.size + sizeof(struct heapPhy [1]));
        tmp->phy.prev = curr->phy.prev;
    } else {                                                                    /* Previous and next blocks are allocated                   */
        curr->free.next = heapMem->sentinel->free.next;
        curr->free.prev = heapMem->sentinel;
        curr->free.prev->free.next = curr;
        curr->free.next->free.prev = curr;
    }

    return (ES_ERROR_NONE);
}

esError esHeapMemFree(
    struct esHeapMem *  heapMem,
    void *              mem) {

    esError             error;
    esAtomic            intCtx;

    ES_CRITICAL_LOCK_ENTER(&intCtx);
    error = esHeapMemFreeI(
        heapMem,
        mem);
    ES_CRITICAL_LOCK_EXIT(intCtx);

    return (error);
}

esError esHeapGetSizeI(
    struct esHeapMem *  heapMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature == HEAP_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, size != NULL);

    *size = heapMem->size;

    return (ES_ERROR_NONE);
}

esError esHeapGetSize(
    struct esHeapMem *  heapMem,
    size_t *            size) {

    return (esHeapGetSizeI(heapMem, size));
}

esError esHeapGetBlockSizeI(
    struct esHeapMem *  heapMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, heapMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  heapMem->signature == HEAP_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, size != NULL);

    *size = heapMem->size;

    return (ES_ERROR_NONE);
}

esError esHeapGetBlockSize(
    struct esHeapMem *  heapMem,
    size_t *            size) {

    return (esHeapGetBlockSizeI(heapMem, size));
}

#if 0
void esDMemUpdateStatusI(
    esHeapMem *    handle,
    esMemStatus_T *     status) {

    size_t              size;
    size_t              freeTotal;
    size_t              freeAvailable;
    struct heapMemBlock *       curr;

    ES_DBG_API_REQUIRE(ES_DBG_POINTER_NULL, NULL != handle);
    ES_DBG_API_REQUIRE(ES_DBG_OBJECT_NOT_VALID, HEAP_MEM_SIGNATURE == handle->signature);
    ES_DBG_API_REQUIRE(ES_DBG_POINTER_NULL, NULL != status);

    size          = 0u;
    freeTotal     = 0u;
    freeAvailable = 0u;
    curr          = handle->sentinel->phyPrev;

    while (curr != handle->sentinel) {
        size += curr->phySize;

        if (NULL != curr->freeNext) {
            size_t freeSize;

            freeSize = curr->phySize - sizeof(struct heapMemBlock);
            freeTotal += freeSize;

            if (freeSize > freeAvailable) {
                freeAvailable = freeSize;
            }
        }
        curr = curr->phyPrev;
    }
    status->size = size;
    status->freeSpaceTotal = freeTotal;
    status->freeSpaceContiguous = freeAvailable;
}

void esDMemUpdateStatus(
    esHeapMem *    handle,
    esMemStatus_T *     status) {

    esAtomic           intCtx;

    ES_CRITICAL_LOCK_ENTER(&intCtx);
    esDMemUpdateStatusI(
        handle,
        status);
    ES_CRITICAL_LOCK_EXIT(intCtx);
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of heap_mem.c
 ******************************************************************************/
