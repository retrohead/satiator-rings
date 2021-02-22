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



#include "memb.h"
#include <sys/queue.h>

#include <inttypes.h>

#include "slob.h"

/*
 * A C preprocessing macro for concatenating to strings.
 *
 * We need use two macros (CC_CONCAT and CC_CONCAT_S) in order to allow
 * concatenation of two defined macros.
 */
#define CC_CONCAT_S(s1, s2)     s1 ## s2
#define CC_CONCAT(s1, s2)       CC_CONCAT_S(s1, s2)

#ifdef __GNUC__
#define STATIC_ASSERT(exp)                                                     \
   typedef char CC_CONCAT(STATIC_ASSERT_AT_LINE,                          \
   CC_CONCAT(_, __LINE__))[(exp) ? 1 : -1]
#endif /* __GNUC__ */


STATIC_ASSERT(SLOB_PAGE_SIZE >= SLOB_PAGE_BREAK_2ND);

#define SLOB_BLOCK_UNIT         (sizeof(struct slob_block))
#define SLOB_BLOCK_UNITS(s)     (((s) + SLOB_BLOCK_UNIT - 1) / SLOB_BLOCK_UNIT)
#define SLOB_BLOCK_SIZE(s)      ((s) * SLOB_BLOCK_UNIT)

#define SLOB_PAGE(sb)           ((struct slob_page *)((uintptr_t)(sb) &        \
        SLOB_PAGE_MASK))

struct slob_block {
        union {
                /* The start of the address of the request returned to
                 * the requestor must be on a 4 byte boundary due to
                 * alignment issues. */
                int32_t sb_bunits;
                /* Next "member" is a link to the next block relative to
                 * the starting address of the SLOB page. */
                int16_t sb_bnext;
        } s;
};

struct slob_page {
        /* Page must be the first member in order to be aligned on a
         * SLOB page size boundry.
         *
         * +1 unit for the offset of the last block in the SLOB page
         * which terminates the free-list. */
        struct slob_block sp_page[SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE) + 1];
        int16_t sp_bunits; /* Free number of block units */
        struct slob_block *sp_bfree; /* Pointer to first free block */

        TAILQ_ENTRY(slob_page) sp_list; /* Link to next page. */
} __attribute__ ((aligned(SLOB_PAGE_SIZE)));

TAILQ_HEAD(slob_page_list, slob_page);

static struct slob_block *slob_block_alloc(struct slob_page *, int16_t);
static int16_t slob_block_units(struct slob_block *);
static void slob_block_units_set(struct slob_block *, int16_t);

static void slob_block_list_set(struct slob_block *, struct slob_block *, int16_t);
static struct slob_block *slob_block_list_next(struct slob_block *);
static int slob_block_list_last(struct slob_block *) __attribute__ ((unused));

static struct slob_page_list *slob_page_list_select(size_t);

static int slob_page_alloc(struct slob_page_list *);
static int slob_page_free(struct slob_page_list *, struct slob_page *);

static struct slob_page_list
        slob_small_page = TAILQ_HEAD_INITIALIZER(slob_small_page);
static struct slob_page_list
        slob_medium_page = TAILQ_HEAD_INITIALIZER(slob_medium_page);
static struct slob_page_list
        slob_large_page = TAILQ_HEAD_INITIALIZER(slob_large_page);

MEMB(slob_pages, struct slob_page, SLOB_PAGE_COUNT, SLOB_PAGE_SIZE);

/*
 * Initialize the SLOB allocator.
 */
void
slob_init(void)
{
        memb_init(&slob_pages);
}

/*
 * Allocate arbritrary amount of memory.
 */
void *
slob_alloc(size_t size)
{
        struct slob_page_list *spl;
        struct slob_page *sp;

        struct slob_block *block;

        int16_t bunits;
        int16_t btotal;

        /* 1+ unit for the prepended the SLOB header */
        bunits = SLOB_BLOCK_UNITS(size) + 1;
        btotal = SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);

        /* See restrictions */
        if (bunits > btotal)
                return NULL;

        /* Choose the right list of SLOB pages */
        spl = slob_page_list_select(size);

        block = NULL;
        TAILQ_FOREACH(sp, spl, sp_list) {
                /* If the request is too large, there is no need to walk
                 * the free block list. */
                if (bunits > sp->sp_bunits)
                        continue;

                if ((block = slob_block_alloc(sp, bunits)) != NULL)
                        break;
        }

        if (block == NULL) {
                /* There is no SLOB page that can satisfy the request
                 * allocation. */
                if ((slob_page_alloc(spl)) < 0) {
                        /* Completely exhausted pool of SLOB pages. */
                        return NULL;
                }
                /* Okay, let's try again. */
                sp = TAILQ_FIRST(spl);
                block = slob_block_alloc(sp, bunits);
        }

        return block;
}

/*
 * Free memory associated with the pointer ADDR.
 */
void
slob_free(void *addr)
{
        struct slob_page_list *spl;
        struct slob_page *sp;

        struct slob_block *sb;
        struct slob_block *bbase;
        struct slob_block *block;
        struct slob_block *bnext;
        struct slob_block *blast;

        int16_t bunits;

        size_t size;

        if (addr == NULL)
                return;

        sb = (struct slob_block *)addr - 1;
        sp = SLOB_PAGE(sb);

        bunits = slob_block_units(sb);

        /* Case for the just-freed block has emptied the SLOB page of
         * any allocations. */
        if ((sp->sp_bunits + bunits) == SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE)) {
                size = SLOB_BLOCK_SIZE(bunits);
                spl = slob_page_list_select(size);
                slob_page_free(spl, sp);
                return;
        }

        /* Case for when a SLOB page has no free block units. */
        if (sp->sp_bunits == 0) {
                sp->sp_bfree = sb;

                bbase = &sp->sp_page[0];
                blast = bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);
                slob_block_list_set(sb, blast,
                    SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE));
        } else {
                /* Partially full SLOB page; possibly needs coelescing
                 * blocks. */
                block = sp->sp_bfree;
                bnext = slob_block_list_next(block);
                if (sb < block) {
                        slob_block_list_set(sb, block, bunits);
                        sp->sp_bfree = sb;
                } else {
                        /* Walk the free-list until the block adjacent
                         * to the just-freed block is found. */
                        while (sb > bnext) {
                                block = bnext;
                                bnext = slob_block_list_next(block);
                        }

                        if ((block + bunits) == sb) {
                                /* Coelesce adjacent block from the left. */
                                slob_block_list_set(block, bnext,
                                    slob_block_units(block) +
                                    bunits);
                        } else {
                                /* Link BLOCK to SB and SB to BNEXT. */
                                slob_block_list_set(block, sb,
                                    slob_block_units(block));
                                slob_block_list_set(sb, bnext, bunits);
                        }

                        if ((sb + bunits) == bnext) {
                                /* Coelesce adjacent block from the right. */
                                slob_block_list_set(block,
                                    slob_block_list_next(bnext),
                                    slob_block_units(block) +
                                    slob_block_units(bnext));
                        }
                }
        }

        sp->sp_bunits += bunits;
}

/*
 * Link two SLOB blocks together. Namely, have SB link to SB_NEXT.
 */
static void
slob_block_list_set(struct slob_block *sb, struct slob_block *sb_next,
    int16_t units)
{
        struct slob_page *sp;
        int16_t bofs;

        sp = SLOB_PAGE(sb);
        bofs = sb_next - sp->sp_page;

        if (units <= 0)
                sb[0].s.sb_bunits = -bofs;
        else {
                sb[0].s.sb_bunits = units;
                sb[1].s.sb_bnext = bofs;
        }
}

/*
 * Return a pointer to the next free block in the free block linked
 * list.
 */
static struct slob_block *
slob_block_list_next(struct slob_block *sb)
{
        struct slob_page *sp;
        uintptr_t bnext;

        sp = SLOB_PAGE(sb);
        if (sb[0].s.sb_bunits < 0)
                bnext = -sb[0].s.sb_bunits;
        else
                bnext = sb[1].s.sb_bunits;

        return sp->sp_page + bnext;
}

/*
 * Return 1 if the given block is at the tail of the block linked
 * list. Otherwise, return 0.
 */
static int
slob_block_list_last(struct slob_block *sb)
{
        struct slob_page *sp;

        sp = SLOB_PAGE(sb);

        return ((slob_block_list_next(sb)) == sp->sp_page);
}

/*
 * Attempt to allocate a block that is as least as big as BUNITS from a
 * SLOB page SP by first performing the next-fit method.
 *
 * If successful, a pointer to that block is returned. Otherwise, a NULL
 * pointer is returned if there is no block big enough to hold the
 * allocation request.
 */
static struct slob_block *
slob_block_alloc(struct slob_page *sp, int16_t bunits)
{
        struct slob_block *block;
        struct slob_block *bprev;
        struct slob_block *bnext;

        uint16_t bavail;

        bprev = NULL;
        bnext = NULL;
        /* Walk the free block list starting at the first free block
         * (next-fit method). */
        block = sp->sp_bfree;
        for (; ; bprev = block, block = slob_block_list_next(block)) {
                bavail = slob_block_units(block);

                if (bunits > bavail)
                        continue;

                bnext = slob_block_list_next(block);
                if (bunits == bavail) {
                        /* No fragmentation (exact fit). */
                        if (bprev != NULL)
                                slob_block_list_set(bprev, bnext, bunits);
                        sp->sp_bfree = bnext;
                } else if (bunits < bavail) {
                        /* Fragmentation. */
                        /* Make a new free block. */
                        slob_block_list_set(block + bunits, bnext,
                            bavail - bunits);

                        if (bprev != NULL) {
                                /* Have the previous block point to the
                                 * newly created block. */
                                slob_block_list_set(bprev, block + bunits,
                                    slob_block_units(bprev));
                        } else
                                sp->sp_bfree = block + bunits;
                }

                sp->sp_bunits -= bunits;

                /* Record the number of units in the block. */
                slob_block_units_set(block, bunits);

                /* Avoid returning the header as the starting address of
                 * the requested allocation. */
                return block + 1;
        }

        return NULL;
}

/*
 * Return the the size of SB.
 */
static int16_t
slob_block_units(struct slob_block *sb)
{
        return (sb->s.sb_bunits > 0) ? sb->s.sb_bunits : 1;
}

/*
 * Set the number of units to SB.
 */
static void
slob_block_units_set(struct slob_block *sb, int16_t units)
{
        if (units < 0)
                sb->s.sb_bunits = 1;
        else
                sb->s.sb_bunits = units;
}

/*
 * Returns the SLOB page list that the requested allocation should be
 * allocated to.
 */
static struct slob_page_list *
slob_page_list_select(size_t size)
{
        struct slob_page_list *spl;

        if (size < SLOB_PAGE_BREAK_1ST)
                spl = &slob_small_page;
        else if (size < SLOB_PAGE_BREAK_2ND)
                spl = &slob_medium_page;
        else
                spl = &slob_large_page;

        return spl;
}

/*-
 * Allocate, initialize, and append a new SLOB page onto the SLOB page
 * list SPL.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - An invalid SLOB page list was passed
 *   - Exceeded the total amount of SLOB pages
 */
static int
slob_page_alloc(struct slob_page_list *spl)
{
        struct slob_page *sp;
        struct slob_block *bbase;
        struct slob_block *blast;

        if (spl == NULL)
                return -1;

        if ((sp = (struct slob_page *)memb_alloc(&slob_pages)) == NULL)
                return -1;

        bbase = &sp->sp_page[0];
        sp->sp_bunits = SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);
        sp->sp_bfree = bbase;
        /* Initialize free block linked list. */
        blast = bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);
        slob_block_list_set(bbase, blast, SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE));

        TAILQ_INSERT_HEAD(spl, sp, sp_list);

        return 0;
}

/*-
 * Remove a page SP from a SLOB page list SPL.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - An invalid SLOB page list SPL or SLOB page SP was passed
 *   - The SLOB page still has referenced/allocated blocks
 *   - The MEMB allocator wasn't able to free memory reference to SPL
 */
static int
slob_page_free(struct slob_page_list *spl, struct slob_page *sp)
{
        if (spl == NULL)
                return -1;

        if (sp == NULL)
                return -1;

        if (sp->sp_bunits > 0)
                return -1;

        if ((memb_free(&slob_pages, sp)) < 0)
                return -1;

        if (!TAILQ_EMPTY(spl))
                TAILQ_REMOVE(spl, sp, sp_list);

        return 0;
}
