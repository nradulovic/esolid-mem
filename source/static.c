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

#include "plat/critical.h"
#include "base/bitop.h"
#include "mem/static.h"

/*=========================================================  LOCAL MACRO's  ==*/

/**@brief       Signature for static memory manager
 */
#define STATIC_MEM_SIGNATURE            ((esAtomic)0xDEADBEEDU)

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

/**@brief       Module information
 */
static const ES_MODULE_INFO_CREATE("StaticMem", "Static Memory management", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

esError esStaticMemInit(
    esStaticMem *       staticMem,
    void *              storage,
    size_t              storageSize) {

    ES_REQUIRE(ES_API_POINTER, NULL != staticMem);
    ES_REQUIRE(ES_API_POINTER, NULL != storage);
    ES_REQUIRE(ES_API_RANGE, 0u != storageSize);

    staticMem->base = storage;
    staticMem->size = (esRamSize)ES_ALIGN_UP(storageSize, ES_CPU_DEF_DATA_ALIGNMENT);
    staticMem->free = staticMem->size;

    ES_OBLIGATION(staticMem->signature = STATIC_MEM_SIGNATURE);

    return (ES_ERROR_NONE);
}

esError esStaticMemAllocI(
    struct esStaticMem * staticMem,
    size_t              size,
    void **             mem) {

    ES_REQUIRE(ES_API_POINTER, staticMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  staticMem->signature == STATIC_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_RANGE,   size != 0u);
    ES_REQUIRE(ES_API_POINTER, mem != NULL);

    size = ES_ALIGN_UP(size, ES_CPU_DEF_DATA_ALIGNMENT);

    if (size <= staticMem->free) {
        staticMem->free -= size;
        *mem = (void *)&staticMem->base[staticMem->free];

        return (ES_ERROR_NONE);
    }

    return (ES_ERROR_NO_MEMORY);
}

esError esStaticMemAlloc(
    esStaticMem *       staticMem,
    size_t              size,
    void **             mem) {

    esError             error;
    esIntrCtx           intrCtx;

    ES_CRITICAL_LOCK_ENTER(&intrCtx);
    error = esStaticMemAllocI(
        staticMem,
        size,
        mem);
    ES_CRITICAL_LOCK_EXIT(intrCtx);

    return (error);
}

esError esStaticMemGetFreeI(
    esStaticMem *       staticMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, staticMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  staticMem->signature == STATIC_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_RANGE,   size != 0u);

    *size = staticMem->free;

    return (ES_ERROR_NONE);
}

esError esStaticMemGetSizeI(
    esStaticMem *       staticMem,
    size_t *            size) {

    ES_REQUIRE(ES_API_POINTER, staticMem != NULL);
    ES_REQUIRE(ES_API_OBJECT,  staticMem->signature == STATIC_MEM_SIGNATURE);
    ES_REQUIRE(ES_API_RANGE,   size != 0u);

    *size = staticMem->size;

    return (ES_ERROR_NONE);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of static_mem.c
 ******************************************************************************/
