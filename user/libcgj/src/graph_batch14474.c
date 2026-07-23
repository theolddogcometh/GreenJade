/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14474: udx dma ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udx_dma_ok_u_14474(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       UDX DMA map/alloc coherent probe.
 *   uint32_t __gj_udx_dma_ok_u_14474  (alias)
 *   __libcgj_batch14474_marker = "libcgj-batch14474"
 *
 * Exclusive continuum CREATE-ONLY (14471-14480: notify/udx soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14480). Unique gj_udx_dma_ok_u_14474
 * surface only; no multi-def. Distinct from sibling notify/udx soft stubs in this wave. Soft
 * pure-data only; does not link UDX host skeleton. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14474_marker[] = "libcgj-batch14474";

/* UDX/notify soft lamp: always off. */
#define B14474_UDX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14474_ok(void)
{
	return B14474_UDX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udx_dma_ok_u_14474 - udx dma ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_udx_dma_ok_u_14474(void)
{
	(void)NULL;
	return b14474_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udx_dma_ok_u_14474(void)
    __attribute__((alias("gj_udx_dma_ok_u_14474")));
