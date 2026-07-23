/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2322: freestanding NUL-terminated sdbm string
 * hash (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_sdbm_u(const char *s);
 *     - sdbm hash over the C string at s until NUL. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL s -> 0.
 *       Empty string -> 0.
 *   uint32_t __gj_str_hash_sdbm_u  (alias)
 *   __libcgj_batch2322_marker = "libcgj-batch2322"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from gj_sdbm /
 * gj_sdbm_n / sdbm_hash (length-bounded) - unique gj_str_hash_sdbm_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2322_marker[] = "libcgj-batch2322";

/* ---- freestanding helpers ---------------------------------------------- */

/* sdbm walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b2322_sdbm_u(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32H = 0u;
	while (*p != 0u) {
		u32H = (uint32_t)(*p) + (u32H << 6) + (u32H << 16) - u32H;
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_sdbm_u - classic sdbm hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns 0. Does not call libc.
 */
uint32_t
gj_str_hash_sdbm_u(const char *sz)
{
	(void)NULL;
	return b2322_sdbm_u(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_sdbm_u(const char *sz)
    __attribute__((alias("gj_str_hash_sdbm_u")));
