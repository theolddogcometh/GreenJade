/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1339: pack up to eight low-nibbles into a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pack_nibbles(const uint8_t *nibbles, size_t n);
 *     — Pack min(n, 8) low nibbles (nibbles[i] & 0x0f) into a u32 with
 *       nibble i in bits [4*i .. 4*i+3]. NULL nibbles or n == 0 → 0.
 *   uint32_t __gj_u32_pack_nibbles  (alias)
 *   __libcgj_batch1339_marker = "libcgj-batch1339"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1339_marker[] = "libcgj-batch1339";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pack_nibbles — pack up to 8 low-nibbles into one uint32_t.
 */
uint32_t
gj_u32_pack_nibbles(const uint8_t *pNibbles, size_t cN)
{
	uint32_t u32Out;
	size_t i;
	size_t cUse;

	if (pNibbles == NULL || cN == 0u) {
		return 0u;
	}

	cUse = (cN > 8u) ? 8u : cN;
	u32Out = 0u;
	for (i = 0u; i < cUse; i++) {
		u32Out |= ((uint32_t)(pNibbles[i] & 0x0fu)) << (i * 4u);
	}
	return u32Out;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pack_nibbles(const uint8_t *pNibbles, size_t cN)
    __attribute__((alias("gj_u32_pack_nibbles")));
