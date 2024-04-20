/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 2024 Paul Cercueil <paul@crapouillou.net>
 */

#ifndef __LIBUDIV_H__
#define __LIBUDIV_H__

#ifdef _MSC_BUILD
#	define clz32(x)		_lzcnt_u32(x)
#else
#	define clz32(x)		__builtin_clz(x)
#endif

typedef struct {
	unsigned int p;
	unsigned int m;
} udiv_t;

#define UDIV_P(div) \
	(31 - clz32(div) + !!((div) & ((div) - 1)))

#define UDIV_M(div, p) \
	(unsigned int)(((0x1ull << (32 + (p))) + (div) - 1) / (unsigned long long)(div))


static inline udiv_t __udiv_set_divider(unsigned int div)
{
	unsigned int p = UDIV_P(div);
	unsigned int m = UDIV_M(div, p);

	return (udiv_t){ .p = p, .m = m, };
}

#ifdef _MSC_BUILD
#define udiv_set_divider(div) __udiv_set_divider(div)
#else
#define udiv_set_divider(div) \
	__builtin_choose_expr(__builtin_constant_p(div), \
			      (udiv_t){ .p = UDIV_P(div), .m = UDIV_M((div), UDIV_P(div)) }, \
			      __udiv_set_divider(div))
#endif

static inline unsigned int udiv_divide(unsigned int val, udiv_t udiv)
{
	unsigned int q = ((unsigned long long)udiv.m * val) >> 32;
	unsigned int t = ((val - q) >> 1) + q;

	return t >> (udiv.p - 1);
}

#endif /* __LIBUDIV_H__ */
