/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8122: strncpy pads-with-NUL id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_pads_with_nul_id_8122(void);
 *     - Return 1 (strncpy zero-pads remaining bytes when src shorter).
 *   uint32_t __gj_strncpy_pads_with_nul_id_8122  (alias)
 *   __libcgj_batch8122_marker = "libcgj-batch8122"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_pads_with_nul_id_8122 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8122_marker[] = "libcgj-batch8122";

/* Soft catalog: strncpy pads with NUL when src length < n. */
#define B8122_STRNCPY_PADS_WITH_NUL ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8122_strncpy_pads_with_nul_id(void)
{
	return B8122_STRNCPY_PADS_WITH_NUL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_pads_with_nul_id_8122 - soft pads-with-NUL catalog flag.
 *
 * Always returns 1. Catalog id only; does not call strncpy(3).
 * No parent wires.
 */
uint32_t
gj_strncpy_pads_with_nul_id_8122(void)
{
	(void)NULL;
	return b8122_strncpy_pads_with_nul_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_pads_with_nul_id_8122(void)
    __attribute__((alias("gj_strncpy_pads_with_nul_id_8122")));
