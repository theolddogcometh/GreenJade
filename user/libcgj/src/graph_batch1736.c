/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1736: uint64_t div/mod by 10 for decimal print.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_mod_10(uint64_t n, unsigned *rem);
 *     — Returns n / 10. If rem is non-NULL, writes n % 10 (0..9).
 *       rem == NULL skips remainder write.
 *   uint64_t __gj_u64_div_mod_10  (alias)
 *   __libcgj_batch1736_marker = "libcgj-batch1736"
 *
 * Decimal/fixed-print exclusive (emit digits least-significant first).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1736_marker[] = "libcgj-batch1736";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1736_divmod10 — n = 10*q + r with r in [0,9].
 */
static void
b1736_divmod10(uint64_t u64N, uint64_t *pQ, unsigned *pR)
{
	uint64_t u64Q;

	u64Q = u64N / 10ull;
	*pQ = u64Q;
	*pR = (unsigned)(u64N - (u64Q * 10ull));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_mod_10 — quotient n/10; optional remainder n%10.
 *
 * n:   value
 * rem: optional digit remainder out (0..9)
 *
 * Returns the quotient. Remainder is written only when rem != NULL.
 */
uint64_t
gj_u64_div_mod_10(uint64_t u64N, unsigned *pRem)
{
	uint64_t u64Q;
	unsigned uR;

	b1736_divmod10(u64N, &u64Q, &uR);
	if (pRem != NULL) {
		*pRem = uR;
	}
	return u64Q;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_mod_10(uint64_t u64N, unsigned *pRem)
    __attribute__((alias("gj_u64_div_mod_10")));
