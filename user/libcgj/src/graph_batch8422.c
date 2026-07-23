/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8422: FNV-1a 32-bit single-byte fold (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_fnv1a_byte_u_8422(uint32_t h, uint32_t b);
 *     - Fold the low 8 bits of b into running FNV-1a hash h:
 *       h = (h ^ (b & 0xff)) * 16777619. Pure; one-byte step.
 *   uint32_t __gj_hash_fnv1a_byte_u_8422  (alias)
 *   __libcgj_batch8422_marker = "libcgj-batch8422"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Pair with
 * gj_hash_fnv1a_init_8421 for incremental hashing. Distinct from
 * fnv1a_32 buffer walk — unique _u_8422 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8422_marker[] = "libcgj-batch8422";

/* FNV-1a 32-bit prime (public domain constant). */
#define B8422_FNV1A32_PRIME 16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* One FNV-1a 32-bit step: xor low byte then multiply by prime. */
static uint32_t
b8422_fnv1a_byte(uint32_t u32H, uint32_t u32B)
{
	u32H ^= (u32B & 0xffu);
	u32H *= B8422_FNV1A32_PRIME;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fnv1a_byte_u_8422 - fold one byte into FNV-1a 32-bit hash.
 *
 * h: running hash (typically from gj_hash_fnv1a_init_8421)
 * b: byte value; only the low 8 bits participate
 *
 * Returns the updated hash. Stateless pure function.
 */
uint32_t
gj_hash_fnv1a_byte_u_8422(uint32_t u32H, uint32_t u32B)
{
	(void)NULL;
	return b8422_fnv1a_byte(u32H, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_fnv1a_byte_u_8422(uint32_t u32H, uint32_t u32B)
    __attribute__((alias("gj_hash_fnv1a_byte_u_8422")));
