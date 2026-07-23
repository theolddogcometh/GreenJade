/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch422: djb2 classic hash (gj-prefixed surface).
 *
 * Surface (unique symbols only — do not export djb2_hash / __djb2_hash):
 *   uint32_t gj_djb2(const void *data, size_t n);
 *     — Dan Bernstein djb2 over n bytes at data. Seed 5381; step is
 *       hash * 33 + c. NULL data with n != 0 → 0. Empty (n == 0) → 5381.
 *   __gj_djb2  (alias)
 *   __libcgj_batch422_marker = "libcgj-batch422"
 *
 * Distinct from batch42 djb2_hash / __djb2_hash. This unit only defines
 * gj_djb2 and its alias.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch422_marker[] = "libcgj-batch422";

/* djb2 initial seed (classic public algorithm). */
#define B422_DJB2_SEED 5381u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_djb2 — classic djb2 hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the seed 5381.
 */
uint32_t
gj_djb2(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = B422_DJB2_SEED;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)p[i];
	}
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_djb2(const void *data, size_t n)
    __attribute__((alias("gj_djb2")));
