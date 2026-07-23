/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14475: udx mmio ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udx_mmio_ok_u_14475(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       UDX ioremap/readl/writel probe.
 *   uint32_t __gj_udx_mmio_ok_u_14475  (alias)
 *   __libcgj_batch14475_marker = "libcgj-batch14475"
 *
 * Exclusive continuum CREATE-ONLY (14471-14480: notify/udx soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14480). Unique gj_udx_mmio_ok_u_14475
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

const char __libcgj_batch14475_marker[] = "libcgj-batch14475";

/* UDX/notify soft lamp: always off. */
#define B14475_UDX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14475_ok(void)
{
	return B14475_UDX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udx_mmio_ok_u_14475 - udx mmio ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_udx_mmio_ok_u_14475(void)
{
	(void)NULL;
	return b14475_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udx_mmio_ok_u_14475(void)
    __attribute__((alias("gj_udx_mmio_ok_u_14475")));
