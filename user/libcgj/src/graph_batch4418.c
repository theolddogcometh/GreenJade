/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4418: binary index to one-hot uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bin_to_onehot_u(uint32_t i);
 *     - Returns 1u << i when i < 32; otherwise returns 0 (out of range).
 *   uint32_t __gj_u32_bin_to_onehot_u  (alias)
 *   __libcgj_batch4418_marker = "libcgj-batch4418"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_bin_to_onehot_u surface only; no multi-def. Complements
 * gj_u32_onehot_to_bin_u (batch4419). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4418_marker[] = "libcgj-batch4418";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4418_bin_to_onehot(uint32_t u32I)
{
	if (u32I >= 32u) {
		return 0u;
	}
	return 1u << u32I;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bin_to_onehot_u - encode bit index i as a single-bit mask.
 *
 * i: bit position (0 = LSB)
 *
 * Returns (1u << i) for i in [0, 31]; returns 0 when i >= 32.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bin_to_onehot_u(uint32_t u32I)
{
	(void)NULL;
	return b4418_bin_to_onehot(u32I);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bin_to_onehot_u(uint32_t u32I)
    __attribute__((alias("gj_u32_bin_to_onehot_u")));
