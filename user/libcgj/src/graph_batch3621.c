/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3621: checked uint64_t remainder (mod).
 *
 * Surface (unique symbols):
 *   int gj_u64_checked_mod_u(uint64_t a, uint64_t b, uint64_t *out);
 *     - Compute a % b. On success write the remainder to *out and
 *       return 0. On divisor zero or NULL out leave *out untouched
 *       and return -1.
 *   int __gj_u64_checked_mod_u  (alias)
 *   __libcgj_batch3621_marker = "libcgj-batch3621"
 *
 * CREATE-ONLY exclusive continuum wave (3621-3630). Unique
 * gj_u64_checked_mod_u surface only; no multi-def. Distinct from
 * gj_u64_mod_u (batch2036) and gj_mod_u64 (batch430) which soft-return
 * 0 on zero divisor. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3621_marker[] = "libcgj-batch3621";

/* ---- freestanding helpers ---------------------------------------------- */

/* a % b when b != 0. */
static uint64_t
b3621_mod(uint64_t u64A, uint64_t u64B)
{
	return u64A % u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_checked_mod_u - a % b with zero-divisor and NULL-out checks.
 *
 * a:   dividend
 * b:   divisor (0 → -1, no store)
 * out: result pointer (NULL → -1)
 *
 * Returns 0 and stores a % b on success; -1 on failure (no store).
 * No parent wires.
 */
int
gj_u64_checked_mod_u(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	(void)NULL;
	if (pOut == NULL) {
		return -1;
	}
	if (u64B == 0ull) {
		return -1;
	}
	*pOut = b3621_mod(u64A, u64B);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_checked_mod_u(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
    __attribute__((alias("gj_u64_checked_mod_u")));
