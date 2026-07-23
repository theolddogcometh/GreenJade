/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14576: udx work ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udx_work_ok_u_14576(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       UDX work probe for the udx soft continuum.
 *   uint32_t __gj_udx_work_ok_u_14576  (alias)
 *   __libcgj_batch14576_marker = "libcgj-batch14576"
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

const char __libcgj_batch14576_marker[] = "libcgj-batch14576";

/* UDX work soft lamp: always off. */
#define B14576_UDX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14576_ok(void)
{
	return B14576_UDX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udx_work_ok_u_14576 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_udx_work_ok_u_14576(void)
{
	(void)NULL;
	return b14576_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udx_work_ok_u_14576(void)
    __attribute__((alias("gj_udx_work_ok_u_14576")));
