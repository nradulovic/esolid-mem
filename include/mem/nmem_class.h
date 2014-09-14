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
 * @brief       Interface of mem_class.
 * @defgroup    def_group Group name
 * @brief       Group brief
 *********************************************************************//** @{ */

#ifndef MEM_CLASS_H_
#define MEM_CLASS_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "base/error.h"
#include "base/debug.h"
#include "mem/heap.h"
#include "mem/pool.h"
#include "mem/static.h"

/*===============================================================  MACRO's  ==*/

#if (CONFIG_API_VALIDATION == 1)
#define ES_MEM_INITIALIZER()                                                    \
    {NULL, {{NULL, 0, 0, 0}}, 0}
#else
#define ES_MEM_INITIALIZER()                                                    \
    {NULL, {{NULL, 0, 0}}}
#endif

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

struct esMem {
    const PORT_C_ROM struct esMemClass * memClass;
    union esMemHandle {
        struct esHeapMem heapMem;
        struct esPoolMem poolMem;
        struct esStaticMem staticMem;
    }                   handle;
#if (CONFIG_API_VALIDATION == 1) || defined(__DOXYGEN__)
    esAtomic            signature;
#endif
};

typedef struct esMem esMem;

/*======================================================  GLOBAL VARIABLES  ==*/

extern const PORT_C_ROM struct esMemClass esGlobalHeapMemClass;
extern const PORT_C_ROM struct esMemClass esGlobalPoolMemClass;
extern const PORT_C_ROM struct esMemClass esGlobalStaticMemClass;

/*===================================================  FUNCTION PROTOTYPES  ==*/

esError esMemInit(
    const PORT_C_ROM struct esMemClass * memClass,
    struct esMem *      object,
    void *              buffer,
    size_t              size,
    size_t              block);

esError esMemTerm(
    struct esMem *      object);

esError esMemAllocI(
    struct esMem *      object,
    size_t              size,
    void **             mem);

esError esMemAlloc(
    struct esMem *      object,
    size_t              size,
    void **             mem);

esError esMemFreeI(
    struct esMem *      object,
    void *              mem);

esError esMemFree(
    struct esMem *      object,
    void *              mem);

esError esMemGetSizeI(
    struct esMem *      object,
    size_t *            size);

esError esMemGetSize(
    struct esMem *      object,
    size_t *            size);

esError esMemGetFreeI(
    struct esMem *      object,
    size_t *            free);

esError esMemGetFree(
    struct esMem *      object,
    size_t *            free);

esError esMemGetBlockSizeI(
    struct esMem *      object,
    size_t *            blockSize);

esError esMemGetBlockSize(
    struct esMem *      object,
    size_t *            blockSize);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of mem_class.h
 ******************************************************************************/
#endif /* MEM_CLASS_H_ */
