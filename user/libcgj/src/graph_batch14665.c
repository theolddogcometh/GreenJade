/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14665: aarch64 sve ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aarch64_sve_ok_u_14665(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       aarch64 sve probe for the aarch64 soft continuum.
 *   uint32_t __gj_aarch64_sve_ok_u_14665  (alias)
 *   __libcgj_batch14665_marker = "libcgj-batch14665"
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

const char __libcgj_batch14665_marker[] = "libcgj-batch14665";

/* AArch64 soft lamp: always off. */
#define B14665_A64  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14665_ok(void)
{
	return B14665_A64;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aarch64_sve_ok_u_14665 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_aarch64_sve_ok_u_14665(void)
{
	(void)NULL;
	return b14665_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aarch64_sve_ok_u_14665(void)
    __attribute__((alias("gj_aarch64_sve_ok_u_14665")));
