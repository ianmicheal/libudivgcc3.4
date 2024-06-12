/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 2024 Paul Cercueil <paul@crapouillou.net>
 */

/*Ian micheal back ported gcc3.4 version
  c89 safe Note that the performance of the optimized division
  functions may be slightly lower than the original implementation
  that used compiler built-ins, but it should still provide a 
  reasonable improvement over the standard long division algorithm.
  6/12/2024
 */

#ifndef __LIBUDIV_H__
#define __LIBUDIV_H__

#include <limits.h>

typedef struct {
    unsigned int p;
    unsigned int m;
} udiv_t;

static inline unsigned int clz32(unsigned int x)
{
    unsigned int n = 0;
    if (x == 0) return 32;
    if ((x & 0xFFFF0000) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000) == 0) { n += 8; x <<= 8; }
    if ((x & 0xF0000000) == 0) { n += 4; x <<= 4; }
    if ((x & 0xC0000000) == 0) { n += 2; x <<= 2; }
    if ((x & 0x80000000) == 0) { n += 1; }
    return n;
}

#define UDIV_P(div) \
    (31 - clz32(div) + !!((div) & ((div) - 1)))

#define UDIV_M(div, p) \
    ((p) == 0x20 ? (div) : \
     (unsigned int)(((0x1ull << (32 + (p))) + (div) - 1) / (unsigned long long)(div)))

static inline udiv_t __udiv_set_divider(unsigned int div)
{
    udiv_t udiv;
    unsigned int p = UDIV_P(div);
    unsigned int m = UDIV_M(div, p);

    udiv.p = p;
    udiv.m = m;

    return udiv;
}

#define udiv_set_divider(div) __udiv_set_divider(div)

static inline unsigned int udiv_divide_fast(unsigned int val, udiv_t udiv)
{
    unsigned int q, t;

    /* This algorithm only works for values 1 < div < 0x80000001. */

    q = ((unsigned long long)udiv.m * val) >> 32;
    t = ((val - q) >> 1) + q;

    return t >> (udiv.p - 1);
}

static inline unsigned int udiv_divide(unsigned int val, udiv_t udiv)
{
    /* Divide by 0x80000001 or higher: the algorithm does not work, so
     * udiv.m contains the full divider value, and we just need to check
     * if the dividend is >= the divider. */
    if (udiv.p == 0x20)
        return val >= udiv.m;

    /* Divide by 1: the algorithm does not work, so handle this special case. */
    if (udiv.m == 0 && udiv.p == 0)
        return val;

    return udiv_divide_fast(val, udiv);
}

#endif /* __LIBUDIV_H__ */
