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

#ifndef ES_HEAP_MEM_H_
#define ES_HEAP_MEM_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <stddef.h>

#include "plat/compiler.h"
#include "base/debug.h"
#include "base/error.h"

/*===============================================================  MACRO's  ==*/

#define ES_HEAP_ALLOC(size)
#define ES_HEAP_FREE(mem)

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/**@brief       Dynamic memory instance handle structure
 * @details     This structure holds information about dynamic memory instance.
 * @see         esHeapMemInit()
 * @api
 */
struct esHeapMem {
    struct heapMemBlock * sentinel;                                             /**<@brief Pointer to the memory sentinel                   */
    size_t              free;
    size_t              size;
#if (1U == CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    esAtomic            signature;                                              /**<@brief Structure signature, used during development only*/
#endif
};

/**@brief       Dynamic memory instance handle type
 * @api
 */
typedef struct esHeapMem esHeapMem;

/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/**
 * @brief       Inicijalizuje dinamican memorijski alokator
 * @param       [out] handle            Deskriptor dinamickog alokatora
 * @param       [in] storage            Predefinisani memorijski prostor koji se
 *                                      predaje dinamickom alokatoru na
 *                                      koriscenje
 * @param       storageSize             Velicina memorijskog prostora u
 *                                      bajtovima
 * @details     Ova funkcija se mora pozvati pre koriscenja funkcija dinamickog
 *              memorijskog alokatora.
 * @warning     Funkcija zahteva da pokazivaci handle i pool budu poravnani
 *              (aligned). Ukoliko se koriste eSolid alokatori za instaciranje
 *              @c handle strukture i @c poolStorage onda je poravnani pristup
 *              osiguran.
 * @warning     Funkcija zahteva da velicina memorijskog prostora @c storageSize
 *              bude poravnana (aligned). Na primer za 32-bitni procesor
 *              (poravnanje 4 bajta): ako je @c storageSize == 313 onda je
 *              potrebno poravnati na sledecu vecu vrednost koja je deljiva sa 4,
 *              u ovom slucaju ce to biti 316.
 * @api
 */
esError esHeapMemInit(
    struct esHeapMem *  heapMem,
    void *              storage,
    size_t              storageSize);

esError esHeapMemTerm(
    struct esHeapMem *  heapMem);

/**
 * @brief       Dodeljuje memorijski prostor velicine @c size
 * @param       [in] handle             Deskriptor dinamickog alokatora
 * @param       size                    Velicina zahtevanog memorijskog prostora
 *                                      u bajtovima.
 * @return      Pokazivac na rezervisani memorijski blok.
 * @details     U debug rezimu ova funkcija uvek vraca pokazivac, odnosno, ne
 *              moze se desiti da vrati NULL pokazivac, kao sto nalaze
 *              standardna implementacija @c malloc C funkcije. Ukoliko se
 *              zahtevana memorija ne moze dobaviti generisace se ASSERT greska.
 *              Kada se ne koristi debug rezim funkcija se ponasa u skladu sa
 *              standardom.
 * @iclass
 */
esError esHeapMemAllocI(
    struct esHeapMem *  heapMem,
    size_t              size,
    void **             mem);

/**
 * @brief       Dodeljuje memorijski prostor velicine @c size
 * @param       [in] handle             Deskriptor dinamickog alokatora
 * @param       size                    Velicina zahtevanog memorijskog prostora
 *                                      u bajtovima.
 * @return      Pokazivac na rezervisani memorijski blok.
 * @details     U debug rezimu ova funkcija uvek vraca pokazivac, odnosno, ne
 *              moze se desiti da vrati NULL pokazivac, kao sto nalaze
 *              standardna implementacija @c malloc C funkcije. Ukoliko se
 *              zahtevana memorija ne moze dobaviti generisace se ASSERT greska.
 *              Kada se ne koristi debug rezim funkcija se ponasa u skladu sa
 *              standardom.
 * @note        Funkcija koristi makroe @ref ES_CRITICAL_LOCK_ENTER i
 *              @ref ES_CRITICAL_LOCK_EXIT za zastitu memorije od istovremenog
 *              pristupa.
 * @api
 */
esError esHeapMemAlloc(
    struct esHeapMem *  heapMem,
    size_t              size,
    void **             mem);

/**
 * @brief       Reciklira memorijski prostor na koji pokazije @c mem
 *              pokazivac
 * @param       [in] handle             Deskriptor dinamickog alokatora
 * @param       [in] mem                Pokazivac na prethodno dodeljen
 *                                      memorijski prostor.
 * @iclass
 */
esError esHeapMemFreeI(
    struct esHeapMem *  heapMem,
    void *              mem);

/**
 * @brief       Reciklira memorijski prostor na koji pokazije @c mem
 *              pokazivac
 * @param       [in] handle             Deskriptor dinamickog alokatora
 * @param       [in] mem                Pokazivac na prethodno dodeljen
 *                                      memorijski prostor.
 * @note        Funkcija koristi makroe @ref ES_CRITICAL_LOCK_ENTER i
 *              @ref ES_CRITICAL_LOCK_EXIT za zastitu memorije od istovremenog
 *              pristupa.
 * @api
 */
esError esHeapMemFree(
    struct esHeapMem *  heapMem,
    void *              mem);

esError esHeapGetSizeI(
    struct esHeapMem *  heapMem,
    size_t *            size);

esError esHeapGetSize(
    struct esHeapMem *  heapMem,
    size_t *            size);

esError esHeapGetBlockSizeI(
    struct esHeapMem *  heapMem,
    size_t *            size);

esError esHeapGetBlockSize(
    struct esHeapMem *  heapMem,
    size_t *            size);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//** @} *//*********************************************
 * END of heap_mem.h
 ******************************************************************************/
#endif /* ES_HEAP_MEM_H_ */
