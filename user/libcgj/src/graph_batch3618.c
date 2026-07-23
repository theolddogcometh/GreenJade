/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3618: checked uint64_t division (0 / -1) (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_checked_div_u(uint64_t a, uint64_t b, uint64_t *out);
 *     - Compute a / b. On success write the quotient to *out and
 *       return 0. On divide-by-zero leave *out untouched and return -1.
 *       out == NULL -> return -1 (no write).
 *   int __gj_u64_checked_div_u  (alias)
 *   __libcgj_batch3618_marker = "libcgj-batch3618"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u64_checked_div_u surface only; no multi-def. Distinct from
 * gj_u64_div_floor (batch2034, soft 0 on /0), gj_u64_div_rem_u
 * (batch3616), and gj_u32_checked_div_u (batch3617). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3618_marker[] = "libcgj-batch3618";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked a/b into *out; 0 success, -1 on /0 or NULL out. */
static int
b3618_checked_div(uint64_t u64A, uint64_t u64B, uint64_t *pu64Out)
{
	if (pu64Out == NULL) {
		return -1;
	}
	if (u64B == 0ull) {
		return -1;
	}
	*pu64Out = u64A / u64B;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_checked_div_u - divide a by b with failure on zero divisor.
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
gj_u64_checked_div_u(uint64_t u64A, uint64_t u64B, uint64_t *pu64Out)
{
	(void)NULL;
	return b3618_checked_div(u64A, u64B, pu64Out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_checked_div_u(uint64_t u64A, uint64_t u64B, uint64_t *pu64Out)
    __attribute__((alias("gj_u64_checked_div_u")));
