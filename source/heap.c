/*
 * This file is part of Embedded Solid.
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

#include "port/sys_lock.h"
#include "port/cpu.h"
#include "shared/component.h"
#include "shared/debug.h"
#include "lib/bitop.h"
#include "mem/heap.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for dynamic memory manager
 */
#define HEAP_MEM_SIGNATURE              ((ncpu_reg)0xdeadbee1u)

/*======================================================  LOCAL DATA TYPES  ==*/

/**@brief       Dynamic allocator memory block header structure
 */
struct PORT_C_ALIGN(NCPU_DATA_ALIGNMENT) n_heap_block
{
    struct heap_phy
    {
        struct n_heap_block *       prev;
        ncpu_ssize                  size;
    }                           phy;
    struct heap_free
    {
        struct n_heap_block *       next;
        struct n_heap_block *       prev;
    }                           free;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static const NCOMPONENT_DEFINE("Heap Memory Management", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


void nheap_init(
    struct nheap *              heap,
    void *                      storage,
    size_t                      size)
{
    struct n_heap_block *        begin;

    NREQUIRE(NAPI_POINTER, heap != NULL);
    NREQUIRE(NAPI_OBJECT,  heap->signature != HEAP_MEM_SIGNATURE);
    NREQUIRE(NAPI_POINTER, storage != NULL);
    NREQUIRE(NAPI_RANGE,   size > sizeof(struct n_heap_block [2]));
    NREQUIRE(NAPI_RANGE,   size < NCPU_SSIZE_MAX);

    size = NALIGN(size, NCPU_DATA_ALIGNMENT);
                                              /* Sentinel is the last element */
    heap->sentinel = (struct n_heap_block *)((uint8_t *)storage + size) - 1;
    begin = (struct n_heap_block *)storage;
    begin->phy.size  = (ncpu_ssize)size;
    begin->phy.size -= (ncpu_ssize)sizeof(struct n_heap_block [1]);
    begin->phy.size -= (ncpu_ssize)sizeof(struct heap_phy [1]);
    begin->phy.prev  = heap->sentinel;
    begin->free.next = heap->sentinel;
    begin->free.prev = heap->sentinel;

    heap->sentinel->phy.size  = -1;
    heap->sentinel->phy.prev  = begin;
    heap->sentinel->free.next = begin;
    heap->sentinel->free.prev = begin;
    heap->size = (size_t)begin->phy.size;
    heap->free = heap->size;

    NOBLIGATION(heap->signature = HEAP_MEM_SIGNATURE);
}



void nheap_term(
    struct nheap *            heap)
{
    NREQUIRE(NAPI_POINTER, heap != NULL);
    NREQUIRE(NAPI_OBJECT,  heap->signature == HEAP_MEM_SIGNATURE);

    heap->sentinel = NULL;

    NOBLIGATION(heap->signature = ~HEAP_MEM_SIGNATURE);
}



void * nheap_alloc_i(
    struct nheap *              heap,
    size_t                      size)
{
    struct n_heap_block *         curr;

    NREQUIRE(NAPI_POINTER, heap != NULL);
    NREQUIRE(NAPI_OBJECT,  heap->signature == HEAP_MEM_SIGNATURE);
    NREQUIRE(NAPI_RANGE,   (size != 0u) && (size < NCPU_SSIZE_MAX));

    size = NALIGN_UP(size, sizeof(struct heap_phy [1]));
    curr = heap->sentinel->free.next;

    while (curr != heap->sentinel) {

        if (curr->phy.size >= (ncpu_ssize)size) {

            if (curr->phy.size >
                (ncpu_ssize)(size + sizeof(struct n_heap_block [1]))) {
                struct n_heap_block * tmp;
                void *                mem;

                                       /* Create smaller free block           */
                mem            = (void *)(&curr->free);
                                       /* Point back to the current block     */
                tmp            = (struct n_heap_block *)
                    ((uint8_t *)curr + size + sizeof(struct heap_phy [1]));
                tmp->phy.prev  = curr;
                tmp->phy.size  = curr->phy.size;
                tmp->phy.size -= (ncpu_ssize)size;
                tmp->phy.size -= (ncpu_ssize)sizeof(struct heap_phy [1]);
                                       /* Mark block as allocated             */
                curr->phy.size = (ncpu_ssize)size * (-1);
                                       /* Remove current and add smaller free */
                                       /* block back to free list             */
                tmp->free.next = curr->free.next;
                tmp->free.prev = curr->free.prev;
                tmp->free.next->free.prev = tmp;
                tmp->free.prev->free.next = tmp;
                curr           = (struct n_heap_block *)
                    ((uint8_t *)tmp + tmp->phy.size + sizeof(struct heap_phy [1]));
                                       /* Point to the newly created block    */
                curr->phy.prev = tmp;

                return (mem);
            } else {
                void *               mem;

                                      /* Remove current block from free list  */
                                      /* and mark it block as allocated       */
                mem                        = (void *)(&curr->free);
                curr->free.next->free.prev = curr->free.prev;
                curr->free.prev->free.next = curr->free.next;
                curr->phy.size             = curr->phy.size * (-1);

                return (mem);
            }
        }
        curr = curr->free.next;
    }

    return (NULL);
}



void * nheap_alloc(
    struct nheap *              heap,
    size_t                      size)
{
    nsys_lock                   sys_lock;
    void *                      mem;

    nsys_lock_enter(&sys_lock);
    mem = nheap_alloc_i(heap, size);
    nsys_lock_exit(&sys_lock);

    return (mem);
}



void nheap_free_i(
    struct nheap *              heap,
    void *                      mem)
{
    struct n_heap_block *         curr;
    struct n_heap_block *         tmp;

    NREQUIRE(NAPI_POINTER, heap != NULL);
    NREQUIRE(NAPI_OBJECT,  heap->signature == HEAP_MEM_SIGNATURE);
    NREQUIRE(NAPI_POINTER, mem != NULL);

    curr           = (struct n_heap_block *)
        ((uint8_t *)mem - offsetof(struct n_heap_block, free));
    curr->phy.size = (ncpu_ssize)curr->phy.size * (-1);                         /* Mark block as free                   */
    tmp            = (struct n_heap_block *)
        ((uint8_t *)curr + curr->phy.size + sizeof(struct heap_phy [1]));

    if ((curr->phy.prev->phy.size > 0) && (tmp->phy.size < 0)) {                /* Previous block is free               */
        curr->phy.prev->phy.size  += curr->phy.size;
        curr->phy.prev->phy.size  += (ncpu_ssize)sizeof(struct heap_phy [1]);
        tmp->phy.prev              = curr->phy.prev;
    } else if ((curr->phy.prev->phy.size < 0) && (tmp->phy.size > 0)) {         /* Next block is free                   */
        curr->free.next            = tmp->free.next;
        curr->free.prev            = tmp->free.prev;
        curr->free.prev->free.next = curr;
        curr->free.next->free.prev = curr;
        curr->phy.size            += tmp->phy.size;
        curr->phy.size            += (ncpu_ssize)sizeof(struct heap_phy [1]);
        tmp                        = (struct n_heap_block *)
            ((uint8_t *)curr + curr->phy.size + sizeof(struct heap_phy [1]));
        tmp->phy.prev              = curr;
    } else if ((curr->phy.prev->phy.size > 0) && (tmp->phy.size > 0)) {         /* Previous and next blocks are free    */
        tmp->free.prev->free.next  = tmp->free.next;
        tmp->free.next->free.prev  = tmp->free.prev;
        curr->phy.prev->phy.size  += curr->phy.size + tmp->phy.size;
        curr->phy.prev->phy.size  += (ncpu_ssize)sizeof(struct heap_phy [2]);
        tmp                        = (struct n_heap_block *)
            ((uint8_t *)curr->phy.prev + curr->phy.prev->phy.size +
            sizeof(struct heap_phy [1]));
        tmp->phy.prev              = curr->phy.prev;
    } else {                                                                    /* Previous and next blocks are used    */
        curr->free.next            = heap->sentinel->free.next;
        curr->free.prev            = heap->sentinel;
        curr->free.prev->free.next = curr;
        curr->free.next->free.prev = curr;
    }
}



void nheap_free(
    struct nheap *              heap,
    void *                      mem)
{
    nsys_lock                   sys_lock;

    nsys_lock_enter(&sys_lock);
    nheap_free_i(heap, mem);
    nsys_lock_exit(&sys_lock);
}



size_t nheap_unoccupied_i(
    const struct nheap *        heap)
{
    (void)heap;

    /*
     * TODO: Implementation missing
     */

    return (0);
}



size_t nheap_unoccupied(
    const struct nheap *        heap)
{
    nsys_lock                   sys_lock;
    size_t                      unoccupied;

    nsys_lock_enter(&sys_lock);
    unoccupied = nheap_unoccupied_i(heap);
    nsys_lock_exit(&sys_lock);

    return (unoccupied);
}



size_t nheap_size_i(
    const struct nheap *        heap)
{
    (void)heap;

    /*
     * TODO: Implementation missing
     */

    return (0);
}



size_t nheap_size(
    const struct nheap *        heap)
{
    nsys_lock                   sys_lock;
    size_t                      size;

    nsys_lock_enter(&sys_lock);
    size = nheap_unoccupied_i(heap);
    nsys_lock_exit(&sys_lock);

    return (size);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of heap_mem.c
 ******************************************************************************/
