/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8115: strcpy need-fits-destination stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_need_fits_u_8115(uint32_t need, uint32_t dst_sz);
 *     - Return 1 if need <= dst_sz (copy including NUL fits), else 0.
 *   uint32_t __gj_strcpy_need_fits_u_8115  (alias)
 *   __libcgj_batch8115_marker = "libcgj-batch8115"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_need_fits_u_8115 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8115_marker[] = "libcgj-batch8115";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8115_need_fits(uint32_t u32Need, uint32_t u32DstSz)
{
	return (u32Need <= u32DstSz) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_need_fits_u_8115 - 1 if byte-need fits in destination.
 *
 * need:   bytes that strcpy would write (payload + NUL)
 * dst_sz: destination capacity in octets
 *
 * Returns 1 when need <= dst_sz, else 0. Soft catalog test; does not
 * call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_need_fits_u_8115(uint32_t u32Need, uint32_t u32DstSz)
{
	(void)NULL;
	return b8115_need_fits(u32Need, u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_need_fits_u_8115(uint32_t u32Need, uint32_t u32DstSz)
    __attribute__((alias("gj_strcpy_need_fits_u_8115")));
