/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14461: aarch64 ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aarch64_ok_u_14461(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       aarch64 architecture/runtime probe.
 *   uint32_t __gj_aarch64_ok_u_14461  (alias)
 *   __libcgj_batch14461_marker = "libcgj-batch14461"
 *
 * Exclusive continuum CREATE-ONLY (14461-14470: aarch64 soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14470). Unique gj_aarch64_ok_u_14461
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

const char __libcgj_batch14461_marker[] = "libcgj-batch14461";

/* AArch64 soft lamp: always off. */
#define B14461_AARCH64  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14461_ok(void)
{
	return B14461_AARCH64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aarch64_ok_u_14461 - aarch64 ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_aarch64_ok_u_14461(void)
{
	(void)NULL;
	return b14461_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aarch64_ok_u_14461(void)
    __attribute__((alias("gj_aarch64_ok_u_14461")));
