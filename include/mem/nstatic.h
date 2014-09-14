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
 * @brief       Static Memory Management module header
 * @defgroup    mem_static Static Memory management
 * @brief       Static Memory Management
 *********************************************************************//** @{ */
/**@defgroup    mem_static_intf Interface
 * @brief       Static Memory Management API
 * @{ *//*--------------------------------------------------------------------*/

#ifndef ES_STATIC_MEM_
#define ES_STATIC_MEM_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>
#include <stdint.h>

#include "plat/compiler.h"
#include "family/profile.h"
#include "base/debug.h"
#include "base/error.h"

/*===============================================================  MACRO's  ==*/

/*------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Static memory instance handle structure
 * @details     This structure holds information about static memory instance.
 * @api
 */
struct esStaticMem {
    uint8_t *           base;                                                   /**<@brief Pointer to the base of static memory        */
    size_t              free;                                                   /**<@brief Current index of managed memory                  */
    size_t              size;                                                   /**<@brief The size of static memory                        */
#if (1U == CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    esAtomic            signature;                                              /**<@brief Structure signature, used during development only*/
#endif
};

/**@brief       Static memory instance handle type
 * @api
 */
typedef struct esStaticMem esStaticMem;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/**@brief       Initializes static memory instance
 * @param       handle
 *              Pointer to handle type variable, see @ref esStaticMem.
 * @param       storage
 *              Storage memory reserved for static memory manager.
 * @param       storageSize
 *              Size of reserved memory expresses in bytes.
 * @details     This function shall be called before any other static memory
 *              management function.
 * @api
 */
esError esStaticMemInit(
    struct esStaticMem * staticMem,
    void *              storage,
    size_t              storageSize);

/**@brief       Allocates static memory of size @c size
 * @param       handle
 *              Pointer to static memory instance, see @ref esStaticMem.
 * @param       size
 *              The size of requested memory in bytes.
 * @return      Pointer to free memory of requested size.
 * @iclass
 */
esError esStaticMemAllocI(
    struct esStaticMem * staticMem,
    size_t              size,
    void **             mem);

/**@brief       Allocates static memory of size @c size
 * @param       handle
 *              Pointer to static memory instance, see @ref esStaticMem.
 * @param       size
 *              The size of requested memory in bytes.
 * @return      Pointer to free memory of requested size.
 * @api
 */
esError esStaticMemAlloc(
    struct esStaticMem * staticMem,
    size_t              size,
    void **             mem);

esError esStaticMemGetFreeI(
    struct esStaticMem * staticMem,
    size_t *            size);

esError esStaticMemGetSizeI(
    esStaticMem *       staticMem,
    size_t *            size);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of static_mem.h
 ******************************************************************************/
#endif /* ES_STATIC_MEM_ */
