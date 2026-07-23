/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2695: shell exclusive-wave soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_shell_wave_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that shell exclusive wave [lo, hi] fits a decade
 *       product span: lo != 0, lo <= hi, and (hi - lo) <= 9 (at most
 *       ten TUs inclusive). Unsigned pure-data form.
 *   int __gj_shell_wave_ok  (alias)
 *   __libcgj_batch2695_marker = "libcgj-batch2695"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_shell_wave_ok surface only; no multi-def. Distinct from
 * gj_shell_ready_p (batch2596 bit), gj_shell_product_ok (batch2497
 * 100-slot), gj_shell_continuum_ok (batch2397), and
 * gj_libcgj_wave_ok (batch2696).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2695_marker[] = "libcgj-batch2695";

/* Soft max decade shell wave width (hi - lo inclusive of ten TUs). */
#define B2695_SHELL_WAVE_MAX  9u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible decade shell exclusive wave. */
static int
b2695_wave_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2695_SHELL_WAVE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_wave_ok - soft decade-wave check for shell span [lo, hi].
 *
 * lo: inclusive shell wave start batch id (must be nonzero)
 * hi: inclusive shell wave end batch id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 9, else 0.
 * Example: lo=2691, hi=2700 -> 1. Does not call libc or probe
 * greenjade-shell.
 */
int
gj_shell_wave_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2695_wave_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_wave_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_shell_wave_ok")));
