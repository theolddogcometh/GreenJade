/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2397: shell continuum soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_shell_continuum_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that shell product continuum [lo, hi] is well-formed:
 *       lo != 0, lo <= hi, and (hi - lo) <= 99 (at most a 100-slot
 *       inclusive shell continuum window). Soft pure-data only.
 *   int __gj_shell_continuum_ok  (alias)
 *   __libcgj_batch2397_marker = "libcgj-batch2397"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_shell_continuum_ok surface only; no multi-def. Distinct from
 * gj_batch_span_ok_u (batch2393 decade), gj_continuum_span_ok
 * (batch2294), and gj_shell_ver_ok_hint (batch2295 major.minor).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2397_marker[] = "libcgj-batch2397";

/* Soft max shell continuum width (hi - lo, 100 slots inclusive). */
#define B2397_SHELL_SPAN_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible shell product continuum. */
static int
b2397_shell_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2397_SHELL_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_continuum_ok - soft check for shell continuum [lo, hi].
 *
 * lo: inclusive shell continuum start id (must be nonzero)
 * hi: inclusive shell continuum end id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 99, else 0.
 * Does not call libc or probe greenjade-shell.
 */
int
gj_shell_continuum_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2397_shell_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_continuum_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_shell_continuum_ok")));
