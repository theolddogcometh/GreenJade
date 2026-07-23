/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4083: floor unsigned Q48.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_floor_q16_u(uint64_t q);
 *     - Floor of unsigned Q48.16 q as a plain integer (q >> 16).
 *   uint64_t __gj_u64_floor_q16_u  (alias)
 *   __libcgj_batch4083_marker = "libcgj-batch4083"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u32_floor_q16_u (batch4082) / gj_q16_to_u32_u (batch3063) —
 * unique gj_u64_floor_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4083_marker[] = "libcgj-batch4083";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor(q) for unsigned Q48.16: logical right shift by 16.
 */
static uint64_t
b4083_floor(uint64_t u64Q)
{
	return u64Q >> 16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_floor_q16_u - floor of unsigned Q48.16 q to integer.
 *
 * q: unsigned Q48.16 (bits 63..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns floor(q) in [0, 2^48-1]. No parent wires.
 */
uint64_t
gj_u64_floor_q16_u(uint64_t u64Q)
{
	(void)NULL;
	return b4083_floor(u64Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_floor_q16_u(uint64_t u64Q)
    __attribute__((alias("gj_u64_floor_q16_u")));
