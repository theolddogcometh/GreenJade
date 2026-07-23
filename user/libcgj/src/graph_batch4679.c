/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4679: freestanding 32-bit djb2-like C-string
 * hash (string-measure wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash32_u(const char *s);
 *     - Dan Bernstein djb2-like digest over the C string at s until
 *       NUL. Seed 5381; step is hash * 33 + c. NULL s → 0. Empty
 *       string → 5381.
 *   uint32_t __gj_str_hash32_u  (alias)
 *   __libcgj_batch4679_marker = "libcgj-batch4679"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_hash_djb2_u (batch2321), gj_str_hash_djb2_u2 (batch3182),
 * and other str hash surfaces — unique gj_str_hash32_u only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4679_marker[] = "libcgj-batch4679";

/* djb2-like initial seed (classic public algorithm). */
#define B4679_HASH32_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

/* djb2-like walk of a NUL-terminated C string. NULL → 0. */
static uint32_t
b4679_hash32(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)(const void *)sz;
	u32H = B4679_HASH32_SEED;
	while (*p != (unsigned char)0u) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)(*p);
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash32_u - djb2-like 32-bit hash of a NUL-terminated C string.
 *
 * s: C string (NULL → 0)
 *
 * Returns the 32-bit digest. Empty string returns seed 5381. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_str_hash32_u(const char *s)
{
	(void)NULL;
	return b4679_hash32(s);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash32_u(const char *s)
    __attribute__((alias("gj_str_hash32_u")));
