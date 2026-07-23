/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1731: 128-bit add of u64 with carry-out hint.
 *
 * Surface (unique symbols):
 *   int gj_u128_add_u64_hint(uint64_t *lo, uint64_t *hi, uint64_t add,
 *                            uint64_t *carry_out);
 *     — Treat (*lo, *hi) as little-endian 128-bit (lo = low limb).
 *       Add `add` into the pair in place. Optional *carry_out receives
 *       the 1-bit overflow out of the high limb (0 or 1); NULL skips.
 *       lo/hi NULL → -1 (no write). Returns 0 on success.
 *   int __gj_u128_add_u64_hint  (alias)
 *   __libcgj_batch1731_marker = "libcgj-batch1731"
 *
 * Decimal/fixed-print exclusive wave (batch1731–1739). Unique surface
 * only; no multi-def of prior sat/overflow helpers.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1731_marker[] = "libcgj-batch1731";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1731_add_u64_cout — a + b with carry-out (0/1). Sum may wrap.
 */
static uint64_t
b1731_add_u64_cout(uint64_t u64A, uint64_t u64B, uint64_t *pSum)
{
	uint64_t u64S;

	u64S = u64A + u64B;
	*pSum = u64S;
	return (u64S < u64A) ? 1ull : 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u128_add_u64_hint — (*lo,*hi) += add; optional carry out of hi.
 *
 * lo, hi: in/out limbs (required)
 * add:    unsigned addend
 * carry_out: optional overflow bit from high limb
 *
 * Returns 0 on success, -1 if lo or hi is NULL.
 */
int
gj_u128_add_u64_hint(uint64_t *pLo, uint64_t *pHi, uint64_t u64Add,
    uint64_t *pCarryOut)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64C0;
	uint64_t u64C1;
	uint64_t u64Tmp;

	if (pLo == NULL || pHi == NULL) {
		return -1;
	}

	u64Lo = *pLo;
	u64Hi = *pHi;

	u64C0 = b1731_add_u64_cout(u64Lo, u64Add, &u64Lo);
	u64C1 = b1731_add_u64_cout(u64Hi, u64C0, &u64Tmp);
	u64Hi = u64Tmp;

	*pLo = u64Lo;
	*pHi = u64Hi;
	if (pCarryOut != NULL) {
		*pCarryOut = u64C1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u128_add_u64_hint(uint64_t *pLo, uint64_t *pHi, uint64_t u64Add,
    uint64_t *pCarryOut)
    __attribute__((alias("gj_u128_add_u64_hint")));
