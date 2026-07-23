/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8424: djb2 single-byte fold (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_djb2_byte_u_8424(uint32_t h, uint32_t b);
 *     - Fold the low 8 bits of b into running djb2 hash h:
 *       h = h * 33 + (b & 0xff)  (i.e. ((h << 5) + h) + byte).
 *   uint32_t __gj_hash_djb2_byte_u_8424  (alias)
 *   __libcgj_batch8424_marker = "libcgj-batch8424"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Pair with
 * gj_hash_djb2_init_8423 for incremental hashing. Distinct from
 * djb2_hash buffer walk — unique _u_8424 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8424_marker[] = "libcgj-batch8424";

/* ---- freestanding helpers ---------------------------------------------- */

/* One djb2 step: hash = hash * 33 + low byte. */
static uint32_t
b8424_djb2_byte(uint32_t u32H, uint32_t u32B)
{
	/* ((h << 5) + h) is the classic multiply-by-33 form. */
	u32H = ((u32H << 5) + u32H) + (u32B & 0xffu);
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_djb2_byte_u_8424 - fold one byte into a djb2 hash.
 *
 * h: running hash (typically from gj_hash_djb2_init_8423)
 * b: byte value; only the low 8 bits participate
 *
 * Returns the updated hash. Stateless pure function.
 */
uint32_t
gj_hash_djb2_byte_u_8424(uint32_t u32H, uint32_t u32B)
{
	(void)NULL;
	return b8424_djb2_byte(u32H, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_djb2_byte_u_8424(uint32_t u32H, uint32_t u32B)
    __attribute__((alias("gj_hash_djb2_byte_u_8424")));
