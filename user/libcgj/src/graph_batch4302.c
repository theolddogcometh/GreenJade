/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4302: checked uint32_t right shift (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shr_checked_u(uint32_t x, unsigned n, uint32_t *ok);
 *     - If n >= 32, set *ok = 0 (when ok != NULL) and return 0.
 *       Otherwise set *ok = 1 and return x >> n (logical). ok may be NULL.
 *   uint32_t __gj_u32_shr_checked_u  (alias)
 *   __libcgj_batch4302_marker = "libcgj-batch4302"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_u32_shl_checked_u (batch4301) — unique gj_u32_shr_checked_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4302_marker[] = "libcgj-batch4302";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked logical right shift; n >= 32 fails with 0. ok may be NULL. */
static uint32_t
b4302_shr_checked(uint32_t u32X, unsigned uN, uint32_t *pu32Ok)
{
	if (uN >= 32u) {
		if (pu32Ok != NULL) {
			*pu32Ok = 0u;
		}
		return 0u;
	}
	if (pu32Ok != NULL) {
		*pu32Ok = 1u;
	}
	return u32X >> uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shr_checked_u - checked logical right shift of a 32-bit word.
 *
 * x:  value to shift
 * n:  shift count; n >= 32 -> failure
 * ok: optional success flag (NULL ignored); 1 on success, 0 on failure
 *
 * Returns x >> n for n in 0..31; otherwise 0. No shift UB.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_shr_checked_u(uint32_t u32X, unsigned uN, uint32_t *pu32Ok)
{
	(void)NULL;
	return b4302_shr_checked(u32X, uN, pu32Ok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shr_checked_u(uint32_t u32X, unsigned uN, uint32_t *pu32Ok)
    __attribute__((alias("gj_u32_shr_checked_u")));
