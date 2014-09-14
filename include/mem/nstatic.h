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

#ifndef NSTATIC_H
#define NSTATIC_H

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "base/ndebug.h"

/*===============================================================  MACRO's  ==*/

/*-------------------------------------------------------  C++ extern base  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Static memory instance handle structure
 * @details     This structure holds information about static memory instance.
 * @api
 */
struct nstatic
{
    void *                      base;               /**<@brief Base address   */
    size_t                      free;               /**<@brief Free bytes     */
    size_t                      size;               /**<@brief Size of memory */
#if (CONFIG_DEBUG_API == 1) || defined(__DOXYGEN__)
    ncpu_reg                    signature;          /**<@brief Debug signature*/
#endif
};

/**@brief       Static memory instance handle type
 * @api
 */
typedef struct nstatic nstatic;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/


/**@brief       Initializes static memory instance
 * @param       static_mem
 *              Pointer to handle type variable, see @ref nstatic.
 * @param       storage
 *              Storage memory reserved for static memory manager.
 * @param       size
 *              Size of reserved memory expresses in bytes.
 * @details     This function shall be called before any other static memory
 *              management function.
 * @api
 */
void nstatic_init(
    struct nstatic *            static_mem,
    void *                      storage,
    size_t                      size);



/**@brief       Allocates static memory of get_size @c get_size
 * @param       static_mem
 *              Pointer to static memory instance, see @ref nstatic.
 * @param       size
 *              The size of requested memory in bytes.
 * @return      Pointer to free memory of requested get_size.
 * @iclass
 */
void * nstatic_alloc_i(
    struct nstatic *            static_mem,
    size_t                      size);



/**@brief       Allocates static memory of get_size @c get_size
 * @param       static_mem
 *              Pointer to static memory instance, see @ref nstatic.
 * @param       size
 *              The size of requested memory in bytes.
 * @return      Pointer to free memory of requested get_size.
 * @api
 */
void * nstatic_alloc(
    struct nstatic *            static_mem,
    size_t                      size);



void nstatic_free_i(
    struct nstatic *            static_mem,
    void *                      mem);



#define nstatic_free                        nstatic_free_i



PORT_C_INLINE
size_t nstatic_unoccupied_i(
    const struct nstatic *      static_mem)
{
    return (static_mem->free);
}



PORT_C_INLINE
size_t nstatic_size_i(
    const struct nstatic *      static_mem)
{
    return (static_mem->size);
}

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of static.h
 ******************************************************************************/
#endif /* NSTATIC_H */
