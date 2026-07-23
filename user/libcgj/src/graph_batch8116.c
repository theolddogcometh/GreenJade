/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8116: strcpy destination capacity ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_dst_ok_u_8116(uint32_t dst_sz);
 *     - Return 1 if dst_sz >= soft minimum (1), else 0.
 *   uint32_t __gj_strcpy_dst_ok_u_8116  (alias)
 *   __libcgj_batch8116_marker = "libcgj-batch8116"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_dst_ok_u_8116 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8116_marker[] = "libcgj-batch8116";

/* Soft minimum destination capacity for a valid strcpy bound. */
#define B8116_MIN_DST  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8116_dst_ok(uint32_t u32DstSz)
{
	return (u32DstSz >= B8116_MIN_DST) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_dst_ok_u_8116 - 1 if destination capacity is catalog-ok.
 *
 * dst_sz: candidate destination capacity in octets
 *
 * Returns 1 when dst_sz >= 1 (room for at least a NUL), else 0.
 * Soft catalog test; does not call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_dst_ok_u_8116(uint32_t u32DstSz)
{
	(void)NULL;
	return b8116_dst_ok(u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_dst_ok_u_8116(uint32_t u32DstSz)
    __attribute__((alias("gj_strcpy_dst_ok_u_8116")));
