/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8129: strncpy pad-ok size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_pad_ok_uu_8129(uint32_t src_len, uint32_t n);
 *     - Return 1 for any finite size pair (catalog pad math always ok).
 *   uint32_t __gj_strncpy_pad_ok_uu_8129  (alias)
 *   __libcgj_batch8129_marker = "libcgj-batch8129"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_pad_ok_uu_8129 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8129_marker[] = "libcgj-batch8129";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8129_strncpy_pad_ok(uint32_t u32SrcLen, uint32_t u32N)
{
	/* Catalog: every uint32 length pair is acceptable for pad math. */
	(void)u32SrcLen;
	(void)u32N;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_pad_ok_uu_8129 - 1 if sizes are ok for soft pad math.
 *
 * src_len: length of source string (excluding NUL), catalog units
 * n:       destination bound passed to strncpy
 *
 * Always returns 1 (soft catalog: all size pairs are ok). Does not call
 * strncpy(3). No parent wires.
 */
uint32_t
gj_strncpy_pad_ok_uu_8129(uint32_t u32SrcLen, uint32_t u32N)
{
	(void)NULL;
	return b8129_strncpy_pad_ok(u32SrcLen, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_pad_ok_uu_8129(uint32_t u32SrcLen, uint32_t u32N)
    __attribute__((alias("gj_strncpy_pad_ok_uu_8129")));
