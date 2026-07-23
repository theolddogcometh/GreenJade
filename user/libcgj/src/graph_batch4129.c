/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4129: low-w ones mask for uint32_t bitfields.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bf_width_mask_u(unsigned w);
 *     - Return a right-aligned mask of w consecutive ones in the low
 *       bits (mask for a field of width w). Soft-guard: w==0 yields 0.
 *       w is capped at the type width 32 (w>=32 yields 0xFFFFFFFF).
 *       Never forms a shift-by-32.
 *   uint32_t __gj_u32_bf_width_mask_u  (alias)
 *   __libcgj_batch4129_marker = "libcgj-batch4129"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Unique gj_u32_bf_width_mask_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4129_marker[] = "libcgj-batch4129";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Right-aligned ones mask of width uW, capped at 32.
 * Soft-guard w==0 → 0.
 */
static uint32_t
b4129_width_mask(unsigned uW)
{
	if (uW == 0u) {
		return 0u;
	}
	if (uW >= 32u) {
		return 0xFFFFFFFFu;
	}
	return ((uint32_t)1u << uW) - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bf_width_mask_u - low-w ones mask (capped at 32).
 *
 * w: field width in bits
 *
 * Returns (1<<w)-1 for 0 < w < 32, 0 when w==0, all-ones when w>=32.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bf_width_mask_u(unsigned uW)
{
	(void)NULL;
	return b4129_width_mask(uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bf_width_mask_u(unsigned uW)
    __attribute__((alias("gj_u32_bf_width_mask_u")));
