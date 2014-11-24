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
 * @brief       Interface of nmem_class.
 * @defgroup    def_group Group name
 * @brief       Group brief
 *********************************************************************//** @{ */

#ifndef MEM_CLASS_H_
#define MEM_CLASS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "shared/debug.h"
#include "mem/heap.h"
#include "mem/pool.h"
#include "mem/static.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

struct nmem_class;

struct nmem
{
    const PORT_C_ROM struct nmem_class * mem_class;
    union nmem_handle
    {
        struct nheap                heap_mem;
        struct npool                pool_mem;
        struct nstatic              static_mem;
    }                           handle;
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    ncpu_reg                    signature;
#endif
};

typedef struct nmem nmem;

/*======================================================  GLOBAL VARIABLES  ==*/

extern const PORT_C_ROM struct nmem_class g_heap_mem_class;
extern const PORT_C_ROM struct nmem_class g_pool_mem_class;
extern const PORT_C_ROM struct nmem_class g_static_mem_class;

/*===================================================  FUNCTION PROTOTYPES  ==*/


void nmem_bind(
    struct nmem *               object,
    const PORT_C_ROM struct nmem_class * mem_class);



void * nmem_alloc_i(
    struct nmem *               object,
    size_t                      size);



void * nmem_alloc(
    struct nmem *               object,
    size_t                      size);



void nmem_free_i(
    struct nmem *               object,
    void *                      mem);




void nmem_free(
    struct nmem *               object,
    void *                      mem);



size_t nget_mem_unoccupied_i(
    struct nmem *               object);



size_t nget_mem_unoccupied(
    struct nmem *               object);



size_t nget_mem_size_i(
    struct nmem *               object);



size_t nget_mem_size(
    struct nmem *               object);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of nmem_class.h
 ******************************************************************************/
#endif /* MEM_CLASS_H_ */
