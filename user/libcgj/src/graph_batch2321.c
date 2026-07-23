/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2321: freestanding NUL-terminated djb2 string
 * hash (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_djb2_u(const char *s);
 *     - Dan Bernstein djb2 over the C string at s until NUL. Seed 5381;
 *       step is hash * 33 + c. NULL s -> 0. Empty string -> 5381.
 *   uint32_t __gj_str_hash_djb2_u  (alias)
 *   __libcgj_batch2321_marker = "libcgj-batch2321"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from gj_djb2 /
 * djb2_hash (length-bounded mem hashes) - unique gj_str_hash_djb2_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2321_marker[] = "libcgj-batch2321";

/* djb2 initial seed (classic public algorithm). */
#define B2321_DJB2_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

/* djb2 walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b2321_djb2_u(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32H = B2321_DJB2_SEED;
	while (*p != 0u) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)(*p);
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_djb2_u - classic djb2 hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns seed 5381. Does not
 * call libc.
 */
uint32_t
gj_str_hash_djb2_u(const char *sz)
{
	(void)NULL;
	return b2321_djb2_u(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_djb2_u(const char *sz)
    __attribute__((alias("gj_str_hash_djb2_u")));
