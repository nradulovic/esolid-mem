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
 * @brief       Heap Memory Management module header
 * @defgroup    mem_heap Heap Memory management
 * @brief       Heap Memory management
 *********************************************************************//** @{ */
/**@defgroup    mem_heap_intf Interface
 * @brief       Heap memory API
 * @{ *//*--------------------------------------------------------------------*/

#ifndef NHEAP_H
#define NHEAP_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "lib/debug.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Forward declation
 * @notapi
 */
struct n_heap_block;

/**@brief       Heap memory instance structure
 * @details     This structure holds information about dynamic memory instance.
 * @see         nheap_init()
 * @api
 */
struct nheap
{
    struct n_heap_block *       sentinel;           /**<@brief Heap sentinel  */
    size_t                      free;
    size_t                      size;
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    ncpu_reg                    signature;          /**<@brief Debug signature*/
#endif
};

/**@brief       Heap memory instance type
 * @api
 */
typedef struct nheap nheap;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


/**@brief       Initialize heap structure instance
 * @param       heap
 *              Pointer to heap structure instance, see @ref nheap.
 * @param       storage
 *              Pointer to reserved memory space. Usually this will be an array
 *              of bytes which are statically alloacated.
 * @param       Size of storage reserved memory in bytes.
 * @details     This function must be called before @c heap structure can be
 *              used by other functions.
 * @api
 */
void nheap_init(
    struct nheap *              heap,
    void *                      storage,
    size_t                      size);



/**@brief       Terminate heap instance
 * @param       heap
 *              Pointer to heap structure instance
 * @api
 */
void nheap_term(
    struct nheap *            heap);



void * nheap_alloc_i(
    struct nheap *              heap,
    size_t                      size);



void * nheap_alloc(
    struct nheap *              heap,
    size_t                      size);



void nheap_free_i(
    struct nheap *              heap,
    void *                      mem);



void nheap_free(
    struct nheap *              heap,
    void *                      mem);



size_t nheap_unoccupied_i(
    const struct nheap *        heap);



size_t nheap_unoccupied(
    const struct nheap *        heap);



size_t nheap_size_i(
    const struct nheap *        heap);



size_t nheap_size(
    const struct nheap *        heap);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of heap_mem.h
 ******************************************************************************/
#endif /* NHEAP_H */
