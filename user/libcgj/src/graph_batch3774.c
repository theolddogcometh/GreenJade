/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3774: Rabin–Karp polynomial window hash (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rabin_hash_u(const uint8_t *data, size_t n,
 *                            uint32_t base, uint32_t mod);
 *     - Horner polynomial hash of data[0..n):
 *         h = (...((data[0]*base + data[1])*base + ...) + data[n-1])
 *       reduced mod mod when mod != 0; natural uint32 wrap when mod==0.
 *       n==0 or NULL data with n>0 → 0. Bytes consumed as 0..255.
 *   uint32_t __gj_rabin_hash_u  (alias)
 *   __libcgj_batch3774_marker = "libcgj-batch3774"
 *
 * Exclusive continuum CREATE-ONLY (3771-3780). Distinct from
 * gj_rabin_karp_roll (batch1224) which only slides a window. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3774_marker[] = "libcgj-batch3774";

/* ---- freestanding helpers ---------------------------------------------- */

/* Horner hash with optional modular reduction. */
static uint32_t
b3774_hash(const uint8_t *pData, size_t uN, uint32_t uBase, uint32_t uMod)
{
	size_t uI;
	uint64_t u64H;
	uint64_t u64Base;
	uint64_t u64Mod;

	u64H = 0u;
	if (uMod == 0u) {
		for (uI = 0u; uI < uN; uI++) {
			u64H = u64H * (uint64_t)uBase + (uint64_t)pData[uI];
			u64H &= 0xffffffffull;
		}
		return (uint32_t)u64H;
	}

	u64Base = (uint64_t)(uBase % uMod);
	u64Mod = (uint64_t)uMod;
	for (uI = 0u; uI < uN; uI++) {
		u64H = (u64H * u64Base + (uint64_t)pData[uI]) % u64Mod;
	}
	return (uint32_t)u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rabin_hash_u — polynomial hash of a byte window.
 *
 * data: window bytes
 * n:    window length
 * base: polynomial base
 * mod:  modulus (0 → uint32 natural wrap)
 *
 * Returns the window hash. Empty / NULL → 0. No parent wires.
 */
uint32_t
gj_rabin_hash_u(const uint8_t *data, size_t n, uint32_t base, uint32_t mod)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (data == NULL) {
		return 0u;
	}
	return b3774_hash(data, n, base, mod);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rabin_hash_u(const uint8_t *data, size_t n, uint32_t base,
                           uint32_t mod)
    __attribute__((alias("gj_rabin_hash_u")));
