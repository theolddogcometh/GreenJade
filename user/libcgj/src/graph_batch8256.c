/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8256: isolate-lowest-set-bit integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_isolate_lsb_u_8256(uint32_t v);
 *     - Return v & -v: the single lowest set bit of v, or 0 if v == 0.
 *   uint32_t __gj_bit_isolate_lsb_u_8256  (alias)
 *   __libcgj_batch8256_marker = "libcgj-batch8256"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_isolate_lsb_u_8256 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8256_marker[] = "libcgj-batch8256";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8256_isolate_lsb — keep only the least-significant 1-bit of v.
 * Two's-complement identity: v & -v (unsigned wrap of -v is defined).
 */
static uint32_t
b8256_isolate_lsb(uint32_t u32V)
{
	return u32V & (uint32_t)(0u - u32V);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_isolate_lsb_u_8256 - isolate lowest set bit of a 32-bit word.
 *
 * v: value to isolate
 *
 * Returns a power-of-two mask with only the LSB of v set, or 0 when
 * v is 0. Soft isolate catalog surface; pure integer. No parent wires.
 */
uint32_t
gj_bit_isolate_lsb_u_8256(uint32_t u32V)
{
	(void)NULL;
	return b8256_isolate_lsb(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_isolate_lsb_u_8256(uint32_t u32V)
    __attribute__((alias("gj_bit_isolate_lsb_u_8256")));
