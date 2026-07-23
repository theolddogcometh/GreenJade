/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8258: low-n-bits mask integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_mask_u_8258(uint32_t n);
 *     - Return a mask of the low n bits (bits 0..n-1 set). n == 0 → 0;
 *       n >= 32 → 0xffffffff.
 *   uint32_t __gj_bit_mask_u_8258  (alias)
 *   __libcgj_batch8258_marker = "libcgj-batch8258"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_mask_u_8258 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8258_marker[] = "libcgj-batch8258";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8258_mask — low-n-bits mask, fully defined for n in 0..UINT32_MAX.
 * Avoids undefined (1u << 32) by special-casing n >= 32.
 */
static uint32_t
b8258_mask(uint32_t u32N)
{
	if (u32N == 0u) {
		return 0u;
	}
	if (u32N >= 32u) {
		return 0xffffffffu;
	}
	return (1u << u32N) - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_mask_u_8258 - mask with the low n bits set.
 *
 * n: number of low bits to set (0..32+; values >= 32 yield all-ones)
 *
 * Returns ((1u << n) - 1) for n in 1..31, 0 for n == 0, and 0xffffffff
 * for n >= 32. Soft mask catalog surface; pure integer. No parent wires.
 */
uint32_t
gj_bit_mask_u_8258(uint32_t u32N)
{
	(void)NULL;
	return b8258_mask(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_mask_u_8258(uint32_t u32N)
    __attribute__((alias("gj_bit_mask_u_8258")));
