/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3183: freestanding NUL-terminated sdbm string
 * hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_sdbm_u2(const char *s);
 *     - sdbm hash over the C string at s until NUL. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL s -> 0.
 *       Empty string -> 0. Renamed from gj_str_hash_sdbm_u (collision
 *       with batch2322).
 *   uint32_t __gj_str_hash_sdbm_u2  (alias)
 *   __libcgj_batch3183_marker = "libcgj-batch3183"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_str_hash_sdbm_u2 surface only; no multi-def. Distinct from
 * gj_str_hash_sdbm_u (batch2322). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3183_marker[] = "libcgj-batch3183";

/* ---- freestanding helpers ---------------------------------------------- */

/* sdbm walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b3183_sdbm(const char *sz)
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
 * gj_str_hash_sdbm_u2 - classic sdbm hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_str_hash_sdbm_u2(const char *sz)
{
	(void)NULL;
	return b3183_sdbm(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_sdbm_u2(const char *sz)
    __attribute__((alias("gj_str_hash_sdbm_u2")));
