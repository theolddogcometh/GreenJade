/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4165: freestanding length-bounded djb2 buffer
 * hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_buf_hash_djb2_u2(const void *p, size_t n);
 *     - Dan Bernstein djb2 over n bytes at p. Seed 5381; step is
 *       hash * 33 + c. NULL p with n != 0 -> 0. Empty (n == 0) -> 5381.
 *       Renamed from gj_buf_hash_djb2_u (collision with batch3466).
 *   uint32_t __gj_buf_hash_djb2_u2  (alias)
 *   __libcgj_batch4165_marker = "libcgj-batch4165"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_buf_hash_djb2_u2 surface only; no multi-def. Distinct from
 * gj_buf_hash_djb2_u (batch3466) / gj_mem_hash_djb2_n / gj_str_hash_djb2_u.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4165_marker[] = "libcgj-batch4165";

/* djb2 initial seed (classic public algorithm). */
#define B4165_DJB2_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

/* djb2 over n bytes. NULL data with n != 0 -> 0. */
static uint32_t
b4165_djb2(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32H;
	size_t i;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}
	p = (const unsigned char *)pData;
	u32H = B4165_DJB2_SEED;
	for (i = 0u; i < cb; i++) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)p[i];
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_hash_djb2_u2 - classic djb2 hash of n bytes at p.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit digest. On NULL p with n != 0, returns 0.
 * Empty input (n == 0) returns seed 5381. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_buf_hash_djb2_u2(const void *pData, size_t cb)
{
	(void)NULL;
	return b4165_djb2(pData, cb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_buf_hash_djb2_u2(const void *pData, size_t cb)
    __attribute__((alias("gj_buf_hash_djb2_u2")));
