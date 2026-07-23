/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8249: endian swap-needed tag for wire order.
 *
 * Surface (unique symbols):
 *   uint32_t gj_endian_swap_need_u_8249(void);
 *     - Returns 1: under LE host assumption, multi-byte integers need
 *       an octet swap to/from network (big-endian) wire order.
 *   uint32_t __gj_endian_swap_need_u_8249  (alias)
 *   __libcgj_batch8249_marker = "libcgj-batch8249"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Soft compile-time tag consistent with le_id=1 / be_id=0. Unique
 * gj_endian_swap_need_u_8249 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8249_marker[] = "libcgj-batch8249";

/* Swap needed for BE wire order on LE host. */
#define B8249_SWAP_NEED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8249_swap_need(void)
{
	return B8249_SWAP_NEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_endian_swap_need_u_8249 - whether host/network multi-byte conversion
 * needs an octet swap.
 *
 * Always returns 1 under the continuum LE-host assumption (network is
 * BE). Soft pure data product tag; does not probe memory layout. No
 * parent wires.
 */
uint32_t
gj_endian_swap_need_u_8249(void)
{
	(void)NULL;
	return b8249_swap_need();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_endian_swap_need_u_8249(void)
    __attribute__((alias("gj_endian_swap_need_u_8249")));
