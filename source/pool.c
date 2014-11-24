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

#include "port/sys_lock.h"
#include "port/cpu.h"
#include "shared/component.h"
#include "lib/bitop.h"
#include "mem/pool.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for pool_mem memory manager
 */
#define POOL_MEM_SIGNATURE              ((ncpu_reg)0xdeadbee2u)

/*======================================================  LOCAL DATA TYPES  ==*/

/**@brief       Pool allocator header structure
 */
struct n_pool_block
{
    struct n_pool_block *       next;                                           /**<@brief Next free block              */
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static const NCOMPONENT_DEFINE("Pool Memory Module", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


void npool_init(
    struct npool *              pool,
    void *                      array,
    size_t                      array_size,
    size_t                      block_size)
{
    size_t                      block_cnt;
    size_t                      nblocks;
    struct n_pool_block *         block;

    NREQUIRE(NAPI_POINTER, pool != NULL);
    NREQUIRE(NAPI_POINTER, array != NULL);
    NREQUIRE(NAPI_RANGE,   block_size != 0u);
    NREQUIRE(NAPI_RANGE,   block_size <= array_size);

    block_size = NALIGN_UP(block_size, NCPU_DATA_ALIGNMENT);
    nblocks    = array_size / block_size;
    pool->size = array_size;
    pool->free = array_size;
    pool->block_size = block_size;
    pool->sentinel = (struct n_pool_block *)array;
    block = pool->sentinel;

    for (block_cnt = 0u; block_cnt < nblocks - 1u; block_cnt++) {
        block->next =
            (struct n_pool_block *)((uint8_t *)block + pool->block_size);
        block = block->next;
    }
    block->next = NULL;
    NOBLIGATION(pool->signature = POOL_MEM_SIGNATURE);
}



void * npool_alloc_i(
    struct npool *              pool)
{
    NREQUIRE(NAPI_POINTER, pool != NULL);
    NREQUIRE(NAPI_OBJECT,  pool->signature == POOL_MEM_SIGNATURE);

    if (pool->sentinel != NULL) {
        struct n_pool_block *     block;

        block          = pool->sentinel;
        pool->sentinel = block->next;
        pool->free    -= pool->block_size;

        return ((void *)block);
    } else {
        return (NULL);
    }
}



void * npool_alloc(
    struct npool *              pool)
{
    nsys_lock                   sys_lock;
    void *                      mem;

    nsys_lock_enter(&sys_lock);
    mem = npool_alloc_i(pool);
    nsys_lock_exit(&sys_lock);

    return (mem);
}



void npool_free_i(
    struct npool *              pool,
    void *                      mem)
{
    struct n_pool_block *         block;

    NREQUIRE(NAPI_POINTER, pool != NULL);
    NREQUIRE(NAPI_OBJECT,  pool->signature == POOL_MEM_SIGNATURE);
    NREQUIRE(NAPI_POINTER, mem != NULL);

    block          = (struct n_pool_block *)mem;
    block->next    = pool->sentinel;
    pool->sentinel = block;
    pool->free    += pool->block_size;
}



void npool_free(
    struct npool *              pool,
    void *                      mem)
{
    nsys_lock                 sys_lock;

    nsys_lock_enter(&sys_lock);
    npool_free_i(pool, mem);
    nsys_lock_exit(&sys_lock);
}



size_t npool_unoccupied_i(
    const struct npool *        pool)
{
    (void)pool;

    /*
     * TODO: Implementation missing
     */

    return (0);
}



size_t npool_unoccupied(
    const struct npool *        pool)
{
    nsys_lock                   sys_lock;
    size_t                      unoccupied;

    nsys_lock_enter(&sys_lock);
    unoccupied = npool_unoccupied_i(pool);
    nsys_lock_exit(&sys_lock);

    return (unoccupied);
}



size_t npool_size_i(
    const struct npool *        pool)
{
    (void)pool;

    /*
     * TODO: Implementation missing
     */

    return (0);
}



size_t npool_size(
    const struct npool *        pool)
{
    nsys_lock                   sys_lock;
    size_t                      size;

    nsys_lock_enter(&sys_lock);
    size = npool_unoccupied_i(pool);
    nsys_lock_exit(&sys_lock);

    return (size);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of pool_mem.c
 ******************************************************************************/
