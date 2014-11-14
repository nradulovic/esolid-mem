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
 * @brief       Static Memory Management Implementation
 * @addtogroup  mem_static
 *********************************************************************//** @{ */
/**@defgroup    mem_static_impl Implementation
 * @brief       Static Memory Management Implementation
 * @{ *//*--------------------------------------------------------------------*/

/*=========================================================  INCLUDE FILES  ==*/

#include "plat/sys_lock.h"
#include "lib/module.h"
#include "lib/bitop.h"
#include "mem/static.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for static memory manager
 */
#define STATIC_MEM_SIGNATURE            ((ncpu_reg)0xdeadbee0u)

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

static const NMODULE_INFO_CREATE("Static Memory Management", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/


void nstatic_init(
    struct nstatic *            static_mem,
    void *                      storage,
    size_t                      size)
{
    NREQUIRE(NAPI_POINTER, static_mem != NULL);
    NREQUIRE(NAPI_POINTER, storage != NULL);
    NREQUIRE(NAPI_RANGE,   size > NCPU_DATA_ALIGNMENT);

    static_mem->base = storage;
    static_mem->size = NALIGN(size, NCPU_DATA_ALIGNMENT);
    static_mem->free = static_mem->size;

    NOBLIGATION(static_mem->signature = STATIC_MEM_SIGNATURE);
}



void * nstatic_alloc_i(
    struct nstatic *            static_mem,
    size_t                      size)
{
    NREQUIRE(NAPI_POINTER, static_mem != NULL);
    NREQUIRE(NAPI_POINTER, static_mem->signature == STATIC_MEM_SIGNATURE);

    size = NALIGN_UP(size, NCPU_DATA_ALIGNMENT);

    if (size <= static_mem->free) {
        static_mem->free -= size;

        return ((void *)&((uint8_t *)static_mem->base)[static_mem->free]);
    } else {

        return (NULL);
    }
}



void * nstatic_alloc(
    struct nstatic *            static_mem,
    size_t                      size)
{
    nsys_lock                   sys_lock;
    void *                      mem;

    nsys_lock_enter(&sys_lock);
    mem = nstatic_alloc_i(static_mem, size);
    nsys_lock_exit(&sys_lock);

    return (mem);
}



void nstatic_free_i(
    struct nstatic *            static_mem,
    void *                      mem)
{
    (void)static_mem;
    (void)mem;
    NASSERT_ALWAYS("Called free() for static memory.");
}


/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of static_mem.c
 ******************************************************************************/
