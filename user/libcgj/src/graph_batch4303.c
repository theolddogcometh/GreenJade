/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4303: checked uint64_t left shift (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_checked_u(uint64_t x, unsigned n, uint32_t *ok);
 *     - If n >= 64, set *ok = 0 (when ok != NULL) and return 0.
 *       Otherwise set *ok = 1 and return x << n. ok may be NULL.
 *   uint64_t __gj_u64_shl_checked_u  (alias)
 *   __libcgj_batch4303_marker = "libcgj-batch4303"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_u32_shl_checked_u (batch4301) and gj_u64_shl_sat (batch2405) —
 * unique gj_u64_shl_checked_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4303_marker[] = "libcgj-batch4303";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked left shift; n >= 64 fails with 0. ok may be NULL. */
static uint64_t
b4303_shl_checked(uint64_t u64X, unsigned uN, uint32_t *pu32Ok)
{
	if (uN >= 64u) {
		if (pu32Ok != NULL) {
			*pu32Ok = 0u;
		}
		return 0ULL;
	}
	if (pu32Ok != NULL) {
		*pu32Ok = 1u;
	}
	return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_checked_u - checked left shift of a 64-bit word.
 *
 * x:  value to shift
 * n:  shift count; n >= 64 -> failure
 * ok: optional success flag (NULL ignored); 1 on success, 0 on failure
 *
 * Returns x << n for n in 0..63; otherwise 0. No shift UB.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_shl_checked_u(uint64_t u64X, unsigned uN, uint32_t *pu32Ok)
{
	(void)NULL;
	return b4303_shl_checked(u64X, uN, pu32Ok);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_checked_u(uint64_t u64X, unsigned uN, uint32_t *pu32Ok)
    __attribute__((alias("gj_u64_shl_checked_u")));
