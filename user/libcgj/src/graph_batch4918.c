/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4918: freestanding 32-bit hash of a UUID.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_hash32_u(const uint8_t b[16]);
 *     - FNV-1a 32-bit digest over the 16 UUID octets. NULL -> 0.
 *   uint32_t __gj_uuid_hash32_u  (alias)
 *   __libcgj_batch4918_marker = "libcgj-batch4918"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_str_hash32_u (batch4679) and gj_wang_hash32
 * (batch1019) — unique gj_uuid_hash32_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4918_marker[] = "libcgj-batch4918";

/* FNV-1a 32-bit offset basis and prime (public domain algorithm). */
#define B4918_FNV_OFFSET  2166136261u
#define B4918_FNV_PRIME   16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a over exactly 16 octets. pB must be non-NULL. */
static uint32_t
b4918_hash32(const uint8_t *pB)
{
	uint32_t u32H;
	unsigned i;

	u32H = B4918_FNV_OFFSET;
	for (i = 0u; i < 16u; i++) {
		u32H ^= (uint32_t)pB[i];
		u32H *= B4918_FNV_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_hash32_u - 32-bit FNV-1a hash of a 16-octet UUID.
 *
 * b: 16-octet UUID wire value (NULL -> 0)
 *
 * Returns the 32-bit digest. Fixed 16-octet walk (no NUL scan). Does
 * not call libc. No parent wires.
 */
uint32_t
gj_uuid_hash32_u(const uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0u;
	}
	return b4918_hash32(b);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_hash32_u(const uint8_t b[16])
    __attribute__((alias("gj_uuid_hash32_u")));
