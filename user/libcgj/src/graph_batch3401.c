/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3401: branch-free u32 cmovz (unsigned form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cmovz_u(uint32_t cond, uint32_t a, uint32_t b);
 *     - Branch-free: cond == 0 → a, else b. Pure unsigned API.
 *   uint32_t __gj_u32_cmovz_u  (alias)
 *   __libcgj_batch3401_marker = "libcgj-batch3401"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410): u32_cmovz_u, u64_cmovz_u,
 * u32_cmovnz_u, u64_cmovnz_u, u32_blsi_u, u64_blsi_u, u32_blsr_u,
 * u64_blsr_u, u32_blsmsk_u, batch_id_3410. Distinct from
 * gj_u32_cmov_u (batch3101) / gj_ct_cmov_u32 (batch1512) —
 * unique gj_u32_cmovz_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3401_marker[] = "libcgj-batch3401";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond == 0 → a; else b. Branch-free on a/b. */
static uint32_t
b3401_cmovz(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	uint32_t uZ;
	uint32_t uMask;

	/* mask is all-ones iff cond == 0 */
	uZ = (uint32_t)(uCond == 0u);
	uMask = 0u - uZ;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cmovz_u - branch-free select when cond is zero.
 *
 * cond == 0 → a; cond != 0 → b. Mask derived without a data-dependent
 * branch on a/b. No parent wires.
 */
uint32_t
gj_u32_cmovz_u(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3401_cmovz(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cmovz_u(uint32_t uCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_cmovz_u")));
