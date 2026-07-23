/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4162: freestanding NUL-terminated djb2 string
 * hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_djb2_u3(const char *s);
 *     - Dan Bernstein djb2 over the C string at s until NUL. Seed 5381;
 *       step is hash * 33 + c. NULL s -> 0. Empty string -> 5381.
 *       Renamed from gj_str_hash_djb2_u (collision with batch2321;
 *       u2 owned by batch3182).
 *   uint32_t __gj_str_hash_djb2_u3  (alias)
 *   __libcgj_batch4162_marker = "libcgj-batch4162"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_str_hash_djb2_u3 surface only; no multi-def. Distinct from
 * gj_str_hash_djb2_u (batch2321) and gj_str_hash_djb2_u2 (batch3182).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4162_marker[] = "libcgj-batch4162";

/* djb2 initial seed (classic public algorithm). */
#define B4162_DJB2_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

/* djb2 walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b4162_djb2(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32H = B4162_DJB2_SEED;
	while (*p != 0u) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)(*p);
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_djb2_u3 - classic djb2 hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns seed 5381. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_str_hash_djb2_u3(const char *sz)
{
	(void)NULL;
	return b4162_djb2(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_djb2_u3(const char *sz)
    __attribute__((alias("gj_str_hash_djb2_u3")));
