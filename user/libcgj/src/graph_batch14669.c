/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14669: aarch64 soft ready aggregate lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_aarch64_soft_ready_u_14669(void);
 *     - Returns 0 (soft stub: aarch64 soft aggregate is soft/unprobed).
 *   uint32_t __gj_aarch64_soft_ready_u_14669  (alias)
 *   __libcgj_batch14669_marker = "libcgj-batch14669"
 *
 * Exclusive continuum CREATE-ONLY (14661-14670: aarch64 soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14670). Unique surface
 * only; no multi-def. Distinct from sibling aarch64 soft stubs in prior waves.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14669_marker[] = "libcgj-batch14669";

/* AArch64 soft ready: always off. */
#define B14669_A64  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14669_ok(void)
{
	return B14669_A64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aarch64_soft_ready_u_14669 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_aarch64_soft_ready_u_14669(void)
{
	(void)NULL;
	return b14669_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aarch64_soft_ready_u_14669(void)
    __attribute__((alias("gj_aarch64_soft_ready_u_14669")));
