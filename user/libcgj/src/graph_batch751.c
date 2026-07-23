/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch751: freestanding little-endian 32-bit load.
 *
 * Surface (unique symbols):
 *   uint32_t gj_le32_load(const unsigned char *p);
 *     — Load a 32-bit little-endian value from four bytes at p.
 *       p == NULL → 0.
 *   uint32_t __gj_le32_load  (alias)
 *   __libcgj_batch751_marker = "libcgj-batch751"
 *
 * Distinct from file-static endian helpers (e.g. b70_load_le32,
 * b75_load_le32) which are not exported. Unique gj_le32_load names
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch751_marker[] = "libcgj-batch751";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le32_load — load little-endian uint32_t from four octets at p.
 *
 * p: pointer to at least 4 readable bytes (NULL → return 0).
 *
 * Layout: p[0] is the least-significant byte, p[3] the most.
 * Does not require natural alignment of p.
 */
uint32_t
gj_le32_load(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return (uint32_t)p[0]
	    | ((uint32_t)p[1] << 8)
	    | ((uint32_t)p[2] << 16)
	    | ((uint32_t)p[3] << 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_le32_load(const unsigned char *p)
    __attribute__((alias("gj_le32_load")));
