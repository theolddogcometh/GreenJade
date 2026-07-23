/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8126: strncpy copy-len stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_copy_len_uu_8126(uint32_t src_len, uint32_t n);
 *     - Return min(src_len, n): non-pad bytes copied from src.
 *   uint32_t __gj_strncpy_copy_len_uu_8126  (alias)
 *   __libcgj_batch8126_marker = "libcgj-batch8126"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_copy_len_uu_8126 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8126_marker[] = "libcgj-batch8126";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8126_strncpy_copy_len(uint32_t u32SrcLen, uint32_t u32N)
{
	return (u32SrcLen < u32N) ? u32SrcLen : u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_copy_len_uu_8126 - soft non-pad copy length.
 *
 * src_len: length of source string (excluding NUL), catalog units
 * n:       destination bound passed to strncpy
 *
 * Returns min(src_len, n). Soft catalog math; does not call strncpy(3).
 * No parent wires.
 */
uint32_t
gj_strncpy_copy_len_uu_8126(uint32_t u32SrcLen, uint32_t u32N)
{
	(void)NULL;
	return b8126_strncpy_copy_len(u32SrcLen, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_copy_len_uu_8126(uint32_t u32SrcLen, uint32_t u32N)
    __attribute__((alias("gj_strncpy_copy_len_uu_8126")));
