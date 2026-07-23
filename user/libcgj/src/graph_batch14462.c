/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14462: aarch64 el1 ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aarch64_el1_ok_u_14462(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       aarch64 EL1 exception-level probe.
 *   uint32_t __gj_aarch64_el1_ok_u_14462  (alias)
 *   __libcgj_batch14462_marker = "libcgj-batch14462"
 *
 * Exclusive continuum CREATE-ONLY (14461-14470: aarch64 soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14470). Unique gj_aarch64_el1_ok_u_14462
 * surface only; no multi-def. Distinct from sibling aarch64 soft stubs in this wave. No
 * architecture-specific assembly; pure integer soft tags only. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14462_marker[] = "libcgj-batch14462";

/* AArch64 soft lamp: always off. */
#define B14462_AARCH64  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14462_ok(void)
{
	return B14462_AARCH64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aarch64_el1_ok_u_14462 - aarch64 el1 ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_aarch64_el1_ok_u_14462(void)
{
	(void)NULL;
	return b14462_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aarch64_el1_ok_u_14462(void)
    __attribute__((alias("gj_aarch64_el1_ok_u_14462")));
