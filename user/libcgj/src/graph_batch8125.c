/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8125: strncpy pad-count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncpy_pad_count_uu_8125(uint32_t src_len, uint32_t n);
 *     - Return number of NUL pad bytes (n - src_len if src_len < n else 0).
 *   uint32_t __gj_strncpy_pad_count_uu_8125  (alias)
 *   __libcgj_batch8125_marker = "libcgj-batch8125"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Unique gj_strncpy_pad_count_uu_8125 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8125_marker[] = "libcgj-batch8125";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8125_strncpy_pad_count(uint32_t u32SrcLen, uint32_t u32N)
{
	if (u32SrcLen < u32N)
		return u32N - u32SrcLen;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncpy_pad_count_uu_8125 - soft strncpy NUL pad byte count.
 *
 * src_len: length of source string (excluding NUL), catalog units
 * n:       destination bound passed to strncpy
 *
 * Returns n - src_len when src_len < n, else 0. Soft catalog math;
 * does not call strncpy(3). No parent wires.
 */
uint32_t
gj_strncpy_pad_count_uu_8125(uint32_t u32SrcLen, uint32_t u32N)
{
	(void)NULL;
	return b8125_strncpy_pad_count(u32SrcLen, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncpy_pad_count_uu_8125(uint32_t u32SrcLen, uint32_t u32N)
    __attribute__((alias("gj_strncpy_pad_count_uu_8125")));
