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
 * @brief       Heap Memory Management tests
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mem/heap.h"

static int64_t buffer[19];

int main(
    void) {

    esHeapMem myMem;
    void *    alloc1;
    void *    alloc2;
    void *    alloc3;

    esHeapMemInit(&myMem, buffer, sizeof(buffer));
    /* prim condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc1);
    esHeapMemAlloc(&myMem, 3*8, &alloc2);
    /* sec condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc3);

    /* prev = occ, next = occ */
    esHeapMemFree(&myMem, alloc1);

    /* prev = free, next = occ */
    esHeapMemFree(&myMem, alloc2);
    esHeapMemFree(&myMem, alloc3);

	/* prim condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc1);
    esHeapMemAlloc(&myMem, 3*8, &alloc2);
    /* sec condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc3);

    /* prev = occ, next = occ */
    esHeapMemFree(&myMem, alloc1);
    esHeapMemFree(&myMem, alloc3);

    /* prev = free, next = free */
    esHeapMemFree(&myMem, alloc2);

    /* prim condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc1);
    esHeapMemAlloc(&myMem, 3*8, &alloc2);
    /* sec condition */
    esHeapMemAlloc(&myMem, 2*8, &alloc3);

    /* prev = occ, next = occ */
    esHeapMemFree(&myMem, alloc3);

    /* prev = occ, next = free */
    esHeapMemFree(&myMem, alloc2);

    return EXIT_SUCCESS;
}

void userAssert(
    const struct esDebugReport * dbgReport) {

    while (1);
}
