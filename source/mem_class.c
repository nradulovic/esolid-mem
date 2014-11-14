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
 * @brief       Short desciption of file
 * @addtogroup  module_impl
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "plat/sys_lock.h"
#include "mem/mem_class.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define MEM_SIGNATURE                   ((ncpu_reg)0x01010505ul)

#define MEM_CLASS_SIGNATURE             ((ncpu_reg)0x5a5a5a5aul)

/*======================================================  LOCAL DATA TYPES  ==*/

struct nmem_class
{
    void *                   (* alloc)         (void *, size_t);
    void                     (* free)          (void *, void *);
    size_t                   (* get_unoccupied)(void *);
    size_t                   (* get_size)      (void *);
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    ncpu_reg                    signature;
#endif
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static void null_method(void);

/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/

const PORT_C_ROM struct nmem_class g_heap_mem_class =
{
    (void * (*)(void *, size_t))nheap_alloc_i,
    (void   (*)(void *, void *))nheap_free_i,
    (size_t (*)(void *))        nheap_unoccupied_i,
    (size_t (*)(void *))        nheap_size_i
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

const PORT_C_ROM struct nmem_class g_pool_mem_class =
{
    (void * (*)(void *, size_t))npool_alloc_i,
    (void   (*)(void *, void *))npool_free_i,
    (size_t (*)(void *))        npool_unoccupied_i,
    (size_t (*)(void *))        npool_size_i
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

const PORT_C_ROM struct nmem_class g_static_mem_class =
{
    (void * (*)(void *, size_t))nstatic_alloc_i,
    (void   (*)(void *, void *))null_method,
    (size_t (*)(void *))        nstatic_unoccupied_i,
    (size_t (*)(void *))        nstatic_size_i
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static void null_method(void)
{

}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


void nmem_bind(
    struct nmem *               object,
    const PORT_C_ROM struct nmem_class * mem_class)
{
    object->mem_class = mem_class;
}



void * nmem_alloc_i(
    struct nmem *               object,
    size_t                      size)
{
    return (object->mem_class->alloc(&object->handle, size));
}



void * nmem_alloc(
    struct nmem *               object,
    size_t                      size)
{
    nsys_lock                   sys_lock;
    void *                      mem;

    nsys_lock_enter(&sys_lock);
    mem = nmem_alloc_i(object, size);
    nsys_lock_exit(&sys_lock);

    return (mem);
}



void nmem_free_i(
    struct nmem *               object,
    void *                      mem)
{
    object->mem_class->free(&object->handle, mem);
}



void nmem_free(
    struct nmem *               object,
    void *                      mem)
{
    nsys_lock                   sys_lock;

    nsys_lock_enter(&sys_lock);
    nmem_free_i(object, mem);
    nsys_lock_exit(&sys_lock);
}



size_t nget_mem_unoccupied_i(
    struct nmem *               object)
{
    return (object->mem_class->get_unoccupied(&object->handle));
}



size_t nget_mem_unoccupied(
    struct nmem *               object)
{
    nsys_lock                   sys_lock;
    size_t                      unoccupied;

    nsys_lock_enter(&sys_lock);
    unoccupied = nget_mem_unoccupied_i(object);
    nsys_lock_exit(&sys_lock);

    return (unoccupied);
}



size_t nget_mem_size_i(
    struct nmem *               object)
{
    return (object->mem_class->get_size(&object->handle));
}



size_t nget_mem_size(
    struct nmem *               object)
{
    nsys_lock                   sys_lock;
    size_t                      size;

    nsys_lock_enter(&sys_lock);
    size = nget_mem_size_i(object);
    nsys_lock_exit(&sys_lock);

    return (size);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of nmem_class.c
 ******************************************************************************/
