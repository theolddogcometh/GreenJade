/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14571: udx notify ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udx_notify_ok_u_14571(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       UDX notify probe for the udx soft continuum.
 *   uint32_t __gj_udx_notify_ok_u_14571  (alias)
 *   __libcgj_batch14571_marker = "libcgj-batch14571"
 *
 * Exclusive continuum CREATE-ONLY (14571-14580: udx notify soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14580). Unique surface
 * only; no multi-def. Distinct from sibling udx soft stubs in prior waves.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14571_marker[] = "libcgj-batch14571";

/* UDX notify soft lamp: always off. */
#define B14571_UDX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14571_ok(void)
{
	return B14571_UDX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udx_notify_ok_u_14571 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_udx_notify_ok_u_14571(void)
{
	(void)NULL;
	return b14571_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udx_notify_ok_u_14571(void)
    __attribute__((alias("gj_udx_notify_ok_u_14571")));
