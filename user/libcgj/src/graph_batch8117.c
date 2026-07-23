/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8117: strcpy need ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_need_ok_u_8117(uint32_t need, uint32_t dst_sz);
 *     - Return 1 if need >= 1 and need <= dst_sz, else 0.
 *   uint32_t __gj_strcpy_need_ok_u_8117  (alias)
 *   __libcgj_batch8117_marker = "libcgj-batch8117"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_need_ok_u_8117 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8117_marker[] = "libcgj-batch8117";

/* Soft minimum need: empty source still writes one NUL. */
#define B8117_MIN_NEED  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8117_need_ok(uint32_t u32Need, uint32_t u32DstSz)
{
	if (u32Need < B8117_MIN_NEED)
		return 0u;
	return (u32Need <= u32DstSz) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_need_ok_u_8117 - 1 if need is non-zero and fits in dst_sz.
 *
 * need:   bytes that strcpy would write (payload + NUL)
 * dst_sz: destination capacity in octets
 *
 * Returns 1 when need >= 1 and need <= dst_sz, else 0. Soft catalog
 * test; does not call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_need_ok_u_8117(uint32_t u32Need, uint32_t u32DstSz)
{
	(void)NULL;
	return b8117_need_ok(u32Need, u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_need_ok_u_8117(uint32_t u32Need, uint32_t u32DstSz)
    __attribute__((alias("gj_strcpy_need_ok_u_8117")));
