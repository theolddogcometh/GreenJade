/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8128: strncpy full-pad (empty src) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_full_pad_u_8128(uint32_t n);
 *     - Return pad count for empty source (always n).
 *   uint32_t __gj_strncpy_full_pad_u_8128  (alias)
 *   __libcgj_batch8128_marker = "libcgj-batch8128"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_full_pad_u_8128 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8128_marker[] = "libcgj-batch8128";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8128_strncpy_full_pad(uint32_t u32N)
{
	/* Empty src: all n destination bytes are NUL-padded. */
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_full_pad_u_8128 - soft full-pad count for empty source.
 *
 * n: destination bound passed to strncpy
 *
 * Always returns n. Soft catalog math; does not call strncpy(3).
 * No parent wires.
 */
uint32_t
gj_strncpy_full_pad_u_8128(uint32_t u32N)
{
	(void)NULL;
	return b8128_strncpy_full_pad(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_full_pad_u_8128(uint32_t u32N)
    __attribute__((alias("gj_strncpy_full_pad_u_8128")));
