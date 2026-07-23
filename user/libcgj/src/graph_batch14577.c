/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14577: udx spin ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udx_spin_ok_u_14577(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       UDX spin probe for the udx soft continuum.
 *   uint32_t __gj_udx_spin_ok_u_14577  (alias)
 *   __libcgj_batch14577_marker = "libcgj-batch14577"
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

const char __libcgj_batch14577_marker[] = "libcgj-batch14577";

/* UDX spin soft lamp: always off. */
#define B14577_UDX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14577_ok(void)
{
	return B14577_UDX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udx_spin_ok_u_14577 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_udx_spin_ok_u_14577(void)
{
	(void)NULL;
	return b14577_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udx_spin_ok_u_14577(void)
    __attribute__((alias("gj_udx_spin_ok_u_14577")));
