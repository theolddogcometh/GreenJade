/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1908: window state code soft validity hint.
 *
 * Surface (unique symbols):
 *   int gj_window_state_ok_hint(int state);
 *     — Return 1 if state is in the soft-valid desktop window-state
 *       range [0, 7] (covers normal/minimized/maximized/fullscreen and
 *       common reserved codes), else 0. Soft hint only.
 *   int __gj_window_state_ok_hint  (alias)
 *   __libcgj_batch1908_marker = "libcgj-batch1908"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Unique surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1908_marker[] = "libcgj-batch1908";

/* Soft closed range for window-state enumeration codes. */
#define B1908_STATE_MIN  0
#define B1908_STATE_MAX  7

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1908_state_ok(int nState)
{
	if (nState < B1908_STATE_MIN) {
		return 0;
	}
	if (nState > B1908_STATE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_window_state_ok_hint — 1 iff state is in [0, 7].
 *
 * state: desktop window state code (signed so negatives reject cleanly).
 * Soft catalog-free hint only.
 */
int
gj_window_state_ok_hint(int nState)
{
	(void)NULL;
	return b1908_state_ok(nState);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_window_state_ok_hint(int nState)
    __attribute__((alias("gj_window_state_ok_hint")));
