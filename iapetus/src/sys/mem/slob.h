/*
 * Copyright (c) 2012 Israel Jacquez
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

#ifndef _SLOB_H_
#define _SLOB_H_

#include <stddef.h>

/*-
 * Restrictions
 *
 *   1. Heap size limit: Allocation requests bigger than
 *      SLOB_PAGE_BREAK_2ND cannot be serviced. This is due to the
 *      memory block manager not able to guarantee that sequential
 *      allocations of SLOB pages will be contiguous.
 */

/*
 * Adjust the number of pages to be statically allocated as needed. If
 * memory is quickly exhausted, increase the SLOB page count.
 */ 
#ifndef SLOB_PAGE_COUNT
#define SLOB_PAGE_COUNT 4
#endif /* !SLOB_PAGE_COUNT */

#define SLOB_PAGE_SIZE  0x4000
#define SLOB_PAGE_MASK  (~(SLOB_PAGE_SIZE - 1))

#define SLOB_PAGE_BREAK_1ST 0x0100
#define SLOB_PAGE_BREAK_2ND 0x0400

void slob_init(void);
void *slob_alloc(size_t);
void slob_free(void *);

#endif /* _SLOB_H_ */
