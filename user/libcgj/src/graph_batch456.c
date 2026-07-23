/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch456: Jenkins one-at-a-time hash (gj-prefixed).
 *
 * Surface (unique symbols):
 *   uint32_t gj_jenkins_oaat(const void *data, size_t n);
 *     — Bob Jenkins one-at-a-time (OAAT) over n bytes at data. Seed 0;
 *       per-byte: h += c; h += h << 10; h ^= h >> 6; then avalanche:
 *       h += h << 3; h ^= h >> 11; h += h << 15.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   __gj_jenkins_oaat  (alias)
 *   __libcgj_batch456_marker = "libcgj-batch456"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch456_marker[] = "libcgj-batch456";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jenkins_oaat — Jenkins one-at-a-time hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest after the classic avalanche mix.
 * On NULL data with n != 0, returns 0. Empty input returns 0.
 */
uint32_t
gj_jenkins_oaat(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H = 0u;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	for (i = 0u; i < n; i++) {
		u32H += (uint32_t)p[i];
		u32H += u32H << 10;
		u32H ^= u32H >> 6;
	}
	/* Final avalanche (always applied, including n == 0). */
	u32H += u32H << 3;
	u32H ^= u32H >> 11;
	u32H += u32H << 15;
	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jenkins_oaat(const void *data, size_t n)
    __attribute__((alias("gj_jenkins_oaat")));
