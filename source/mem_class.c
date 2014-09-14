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

#include "plat/critical.h"
#include "mem/mem_class.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define MEM_SIGNATURE                   ((esAtomic)0x01010505ul)

#define MEM_CLASS_SIGNATURE             ((esAtomic)0x5a5a5a5aul)

/*======================================================  LOCAL DATA TYPES  ==*/

struct esMemClass {
    esError          (* init)    (void *, void *, size_t, size_t);
    esError          (* term)    (void *);
    esError          (* alloc)   (void *, size_t, void **);
    esError          (* free)    (void *, void *);
    esError          (* getSize) (void *, size_t *);
    esError          (* getFree) (void *, size_t *);
    esError          (* getBlockSize)(void *, size_t *);
#if (CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    esAtomic            signature;
#endif
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static esError wrapHeapInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block);

static esError wrapPoolInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block);

static esError wrapStaticInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block);

/*=======================================================  LOCAL VARIABLES  ==*/

static const ES_MODULE_INFO_CREATE("mem class", "Memory classes", "Nenad Radulovic");

/*======================================================  GLOBAL VARIABLES  ==*/

const PORT_C_ROM struct esMemClass esGlobalHeapMemClass = {
    wrapHeapInit,
    (esError (*)(void *))esHeapMemTerm,
    (esError (*)(void *, size_t, void **))esHeapMemAllocI,
    (esError (*)(void *, void *))esHeapMemFreeI,
    (esError (*)(void *, size_t *))esHeapGetSizeI,
    NULL,
    (esError (*)(void *, size_t *))esHeapGetBlockSizeI
#if (CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

const PORT_C_ROM struct esMemClass esGlobalPoolMemClass = {
    wrapPoolInit,
    NULL,
    (esError (*)(void *, size_t, void **))esPoolMemAllocI,
    (esError (*)(void *, void *))esPoolMemFreeI,
    NULL,
    NULL,
    NULL
#if (CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

const PORT_C_ROM struct esMemClass esGlobalStaticMemClass = {
    wrapStaticInit,
    NULL,
    (esError (*)(void *, size_t, void **))esStaticMemAllocI,
    NULL,
    (esError (*)(void *, size_t *))esStaticMemGetSizeI,
    NULL,
    NULL
#if (CONFIG_API_VALIDATION) || defined(__DOXYGEN__)
    , MEM_CLASS_SIGNATURE
#endif
};

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static esError wrapHeapInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block) {

    esError             error;

    (void)block;
    error = esHeapMemInit(handle, storage, storageSize);

    return (error);
}

static esError wrapPoolInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block) {

    esError             error;

    error = esPoolMemInit(handle, storage, storageSize, block);

    return (error);
}

static esError wrapStaticInit(
    void *              handle,
    void *              storage,
    size_t              storageSize,
    size_t              block) {

    esError             error;

    (void)block;
    error = esStaticMemInit(handle, storage, storageSize);

    return (error);
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

esError esMemInit(
    const PORT_C_ROM struct esMemClass * memClass,
    struct esMem *      object,
    void *              buffer,
    size_t              size,
    size_t              block) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature != MEM_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, memClass != NULL);
    ES_REQUIRE(ES_API_OBJECT,  memClass->signature == MEM_CLASS_SIGNATURE);
    ES_REQUIRE(ES_API_POINTER, buffer != NULL);

    error = memClass->init(&object->handle, buffer, size, block);

    if (error == ES_ERROR_NONE) {
        object->memClass = memClass;
        ES_OBLIGATION(object->signature = MEM_SIGNATURE);
    }

    return (error);
}

esError esMemTerm(
    struct esMem *      object) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = ES_ERROR_NOT_IMPLEMENTED;

    if (object->memClass->term != NULL) {
        error = object->memClass->term(&object->handle);
    }
    object->memClass = NULL;
    ES_OBLIGATION(object->signature = ~(esAtomic)MEM_SIGNATURE);

    return (error);
}

esError esMemAllocI(
    struct esMem *      object,
    size_t              size,
    void **             mem) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = object->memClass->alloc(&object->handle, size, mem);

    return (error);
}

esError esMemAlloc(
    struct esMem *      object,
    size_t              size,
    void **             mem) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esMemAllocI(object, size, mem);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);
}

esError esMemFreeI(
    struct esMem *      object,
    void *              mem) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = ES_ERROR_NOT_IMPLEMENTED;

    if (object->memClass->free != NULL) {
        error = object->memClass->free(&object->handle, mem);
    }

    return (error);
}

esError esMemFree(
    struct esMem *      object,
    void *              mem) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esMemFreeI(object, mem);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);
}

esError esMemGetSizeI(
    struct esMem *      object,
    size_t *            size) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = ES_ERROR_NOT_IMPLEMENTED;

    if (object->memClass->getSize != NULL) {
        error = object->memClass->getSize(&object->handle, size);
    }

    return (error);
}

esError esMemGetSize(
    struct esMem *      object,
    size_t *            size) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esMemGetSizeI(object, size);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);
}

esError esMemGetFreeI(
    struct esMem *      object,
    size_t *            free) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = ES_ERROR_NOT_IMPLEMENTED;

    if (object->memClass->getFree != NULL) {
        error = object->memClass->getFree(&object->handle, free);
    }

    return (error);

}

esError esMemGetFree(
    struct esMem *      object,
    size_t *            free) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esMemGetFreeI(object, free);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);

}

esError esMemGetBlockSizeI(
    struct esMem *      object,
    size_t *            blockSize) {

    esError             error;

    ES_REQUIRE(ES_API_POINTER, object != NULL);
    ES_REQUIRE(ES_API_OBJECT,  object->signature == MEM_SIGNATURE);

    error = ES_ERROR_NOT_IMPLEMENTED;

    if (object->memClass->getBlockSize != NULL) {
        error = object->memClass->getBlockSize(&object->handle, blockSize);
    }

    return (error);
}

esError esMemGetBlockSize(
    struct esMem *      object,
    size_t *            blockSize) {

    esError             error;
    esLockCtx           lockCtx;

    ES_CRITICAL_LOCK_ENTER(&lockCtx);
    error = esMemGetBlockSizeI(object, blockSize);
    ES_CRITICAL_LOCK_EXIT(lockCtx);

    return (error);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of mem_class.c
 ******************************************************************************/
