/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3617: checked uint32_t division (0 / -1) (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_checked_div_u(uint32_t a, uint32_t b, uint32_t *out);
 *     - Compute a / b. On success write the quotient to *out and
 *       return 0. On divide-by-zero leave *out untouched and return -1.
 *       out == NULL -> return -1 (no write).
 *   int __gj_u32_checked_div_u  (alias)
 *   __libcgj_batch3617_marker = "libcgj-batch3617"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u32_checked_div_u surface only; no multi-def. Distinct from
 * gj_u32_div_floor_u (batch3003, soft 0 on /0) and gj_u32_div_rem_u
 * (batch3615). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3617_marker[] = "libcgj-batch3617";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked a/b into *out; 0 success, -1 on /0 or NULL out. */
static int
b3617_checked_div(uint32_t u32A, uint32_t u32B, uint32_t *pu32Out)
{
	if (pu32Out == NULL) {
		return -1;
	}
	if (u32B == 0u) {
		return -1;
	}
	*pu32Out = u32A / u32B;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_checked_div_u - divide a by b with failure on zero divisor.
 *
 * a:   dividend
 * b:   divisor (0 -> -1, no store)
 * out: result pointer (required; NULL -> -1)
 *
 * Returns 0 if b != 0 and the quotient was stored at *out;
 * returns -1 on divide-by-zero or NULL out (no store on failure).
 * No parent wires.
 */
int
gj_u32_checked_div_u(uint32_t u32A, uint32_t u32B, uint32_t *pu32Out)
{
	(void)NULL;
	return b3617_checked_div(u32A, u32B, pu32Out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_checked_div_u(uint32_t u32A, uint32_t u32B, uint32_t *pu32Out)
    __attribute__((alias("gj_u32_checked_div_u")));
