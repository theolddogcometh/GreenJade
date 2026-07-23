/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8228: string bound overflow predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_overflow_u_8228(uint32_t need, uint32_t dst_sz);
 *     - Return 1 if need > dst_sz (would overflow the destination).
 *   uint32_t __gj_str_overflow_u_8228  (alias)
 *   __libcgj_batch8228_marker = "libcgj-batch8228"
 *
 * Exclusive continuum CREATE-ONLY (8221-8230: string length / bound
 * helpers — strlen_cap, strnlen, strlen_empty, strlen_nonzero,
 * str_bound_ok, str_need_nul, str_fits, str_overflow, str_underflow,
 * batch_id_8230). Unique gj_str_overflow_u_8228 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8228_marker[] = "libcgj-batch8228";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8228_str_overflow(uint32_t u32Need, uint32_t u32DstSz)
{
	return (u32Need > u32DstSz) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_overflow_u_8228 - 1 if need would overflow destination.
 *
 * need:   octets required (payload + NUL)
 * dst_sz: destination capacity in octets
 *
 * Returns 1 when need > dst_sz, else 0. Soft catalog inverse of
 * gj_str_fits_u_8227. No parent wires.
 */
uint32_t
gj_str_overflow_u_8228(uint32_t u32Need, uint32_t u32DstSz)
{
	(void)NULL;
	return b8228_str_overflow(u32Need, u32DstSz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_overflow_u_8228(uint32_t u32Need, uint32_t u32DstSz)
    __attribute__((alias("gj_str_overflow_u_8228")));
