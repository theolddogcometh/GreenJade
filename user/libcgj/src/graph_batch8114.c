/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8114: strcpy destination is-zero stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_dst_is_zero_u_8114(uint32_t dst_sz);
 *     - Return 1 if destination capacity is 0 (cannot hold a NUL), else 0.
 *   uint32_t __gj_strcpy_dst_is_zero_u_8114  (alias)
 *   __libcgj_batch8114_marker = "libcgj-batch8114"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_dst_is_zero_u_8114 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8114_marker[] = "libcgj-batch8114";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8114_dst_is_zero(uint32_t u32DstSz)
{
	return (u32DstSz == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_dst_is_zero_u_8114 - 1 if destination capacity is zero.
 *
 * dst_sz: candidate destination capacity in octets
 *
 * Returns 1 when dst_sz is 0 (no room for a terminating NUL), else 0.
 * Soft catalog test; does not call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_dst_is_zero_u_8114(uint32_t u32DstSz)
{
	(void)NULL;
	return b8114_dst_is_zero(u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_dst_is_zero_u_8114(uint32_t u32DstSz)
    __attribute__((alias("gj_strcpy_dst_is_zero_u_8114")));
