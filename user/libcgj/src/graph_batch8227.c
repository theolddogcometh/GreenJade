/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8227: string need-fits-destination stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_fits_u_8227(uint32_t need, uint32_t dst_sz);
 *     - Return 1 if need <= dst_sz (write including NUL fits), else 0.
 *   uint32_t __gj_str_fits_u_8227  (alias)
 *   __libcgj_batch8227_marker = "libcgj-batch8227"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_str_fits_u_8227 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8227_marker[] = "libcgj-batch8227";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8227_str_fits(uint32_t u32Need, uint32_t u32DstSz)
{
	return (u32Need <= u32DstSz) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_fits_u_8227 - 1 if byte-need fits in destination capacity.
 *
 * need:   octets required (payload + NUL)
 * dst_sz: destination capacity in octets
 *
 * Returns 1 when need <= dst_sz, else 0. Soft catalog test. No parent
 * wires.
 */
uint32_t
gj_str_fits_u_8227(uint32_t u32Need, uint32_t u32DstSz)
{
	(void)NULL;
	return b8227_str_fits(u32Need, u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_fits_u_8227(uint32_t u32Need, uint32_t u32DstSz)
    __attribute__((alias("gj_str_fits_u_8227")));
