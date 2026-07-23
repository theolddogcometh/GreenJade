/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1336: in-place endian swap of a uint64_t array.
 *
 * Surface (unique symbols):
 *   void gj_u64_bswap_arr(uint64_t *a, size_t n);
 *     — For each of a[0..n), reverse eight octets.
 *       NULL a or n == 0 → no-op.
 *   void __gj_u64_bswap_arr  (alias)
 *
 * Distinct from scalar gj_bswap64 (batch417) — array form, unique name.
 *   __libcgj_batch1336_marker = "libcgj-batch1336"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1336_marker[] = "libcgj-batch1336";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1336_bswap64(uint64_t u64X)
{
	return ((u64X & 0x00000000000000ffull) << 56) |
	       ((u64X & 0x000000000000ff00ull) << 40) |
	       ((u64X & 0x0000000000ff0000ull) << 24) |
	       ((u64X & 0x00000000ff000000ull) << 8) |
	       ((u64X & 0x000000ff00000000ull) >> 8) |
	       ((u64X & 0x0000ff0000000000ull) >> 24) |
	       ((u64X & 0x00ff000000000000ull) >> 40) |
	       ((u64X & 0xff00000000000000ull) >> 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bswap_arr — in-place 64-bit octet reverse over a[0..n).
 */
void
gj_u64_bswap_arr(uint64_t *pA, size_t cN)
{
	size_t i;

	if (pA == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		pA[i] = b1336_bswap64(pA[i]);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u64_bswap_arr(uint64_t *pA, size_t cN)
    __attribute__((alias("gj_u64_bswap_arr")));
