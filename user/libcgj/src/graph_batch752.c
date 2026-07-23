/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch752: freestanding big-endian 64-bit load.
 *
 * Surface (unique symbols):
 *   uint64_t gj_be64_load(const unsigned char *p);
 *     — Load a 64-bit big-endian value from eight bytes at p.
 *       p == NULL → 0.
 *   uint64_t __gj_be64_load  (alias)
 *   __libcgj_batch752_marker = "libcgj-batch752"
 *
 * Distinct from le64 helpers and from file-static endian loaders
 * elsewhere. Unique gj_be64_load names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch752_marker[] = "libcgj-batch752";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be64_load — load big-endian uint64_t from eight octets at p.
 *
 * p: pointer to at least 8 readable bytes (NULL → return 0).
 *
 * Layout: p[0] is the most-significant byte, p[7] the least.
 * Does not require natural alignment of p.
 */
uint64_t
gj_be64_load(const unsigned char *p)
{
	if (p == NULL) {
		return 0ULL;
	}
	return ((uint64_t)p[0] << 56)
	    | ((uint64_t)p[1] << 48)
	    | ((uint64_t)p[2] << 40)
	    | ((uint64_t)p[3] << 32)
	    | ((uint64_t)p[4] << 24)
	    | ((uint64_t)p[5] << 16)
	    | ((uint64_t)p[6] << 8)
	    | (uint64_t)p[7];
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_be64_load(const unsigned char *p)
    __attribute__((alias("gj_be64_load")));
