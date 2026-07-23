/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4163: freestanding NUL-terminated sdbm string
 * hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_sdbm_u3(const char *s);
 *     - sdbm hash over the C string at s until NUL. Step is
 *       c + (hash << 6) + (hash << 16) - hash. NULL s -> 0.
 *       Empty string -> 0. Renamed from gj_str_hash_sdbm_u (collision
 *       with batch2322; u2 owned by batch3183).
 *   uint32_t __gj_str_hash_sdbm_u3  (alias)
 *   __libcgj_batch4163_marker = "libcgj-batch4163"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_str_hash_sdbm_u3 surface only; no multi-def. Distinct from
 * gj_str_hash_sdbm_u (batch2322) and gj_str_hash_sdbm_u2 (batch3183).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4163_marker[] = "libcgj-batch4163";

/* ---- freestanding helpers ---------------------------------------------- */

/* sdbm walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b4163_sdbm(const char *sz)
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
 * gj_str_hash_sdbm_u3 - classic sdbm hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_str_hash_sdbm_u3(const char *sz)
{
	(void)NULL;
	return b4163_sdbm(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_sdbm_u3(const char *sz)
    __attribute__((alias("gj_str_hash_sdbm_u3")));
