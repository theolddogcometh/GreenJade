/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2497: shell product continuum soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_shell_product_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that shell product continuum [lo, hi] is well-formed:
 *       lo != 0, lo <= hi, and (hi - lo) <= 99 (at most a 100-slot
 *       inclusive shell product window). Soft pure-data only.
 *   int __gj_shell_product_ok  (alias)
 *   __libcgj_batch2497_marker = "libcgj-batch2497"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_shell_product_ok surface only; no multi-def. Distinct from
 * gj_shell_continuum_ok (batch2397), gj_batch_id_ok_range (batch2493),
 * gj_continuum_span_ok (batch2294), and gj_shell_ver_ok_hint
 * (batch2295 major.minor).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2497_marker[] = "libcgj-batch2497";

/* Soft max shell product width (hi - lo, 100 slots inclusive). */
#define B2497_SHELL_SPAN_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible shell product continuum. */
static int
b2497_shell_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2497_SHELL_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_product_ok - soft check for shell product continuum [lo, hi].
 *
 * lo: inclusive shell product start id (must be nonzero)
 * hi: inclusive shell product end id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 99, else 0.
 * Does not call libc or probe greenjade-shell.
 */
int
gj_shell_product_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2497_shell_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_product_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_shell_product_ok")));
