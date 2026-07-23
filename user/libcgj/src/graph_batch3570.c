/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3570: batch identity for exclusive bitops
 * continuum wave (3561-3570).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3570(void);
 *     - Returns the compile-time graph batch number for this TU (3570).
 *   uint32_t __gj_batch_id_3570  (alias)
 *   __libcgj_batch3570_marker = "libcgj-batch3570"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570:
 * u64_sar_safe_u, u32_bit_count_range_u, u64_bit_count_range_u,
 * u32_find_first_set_u, u64_find_first_set_u, u32_find_last_set_u,
 * u64_find_last_set_u, u32_mask_from_to_u, u64_mask_from_to_u,
 * batch_id_3570). Distinct from prior batch_id_* symbols — unique
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3570_marker[] = "libcgj-batch3570";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3570_id(void)
{
	return 3570u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3570 - report this TU's graph batch number.
 *
 * Always returns 3570. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3570(void)
{
	(void)NULL;
	return b3570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3570(void)
    __attribute__((alias("gj_batch_id_3570")));
