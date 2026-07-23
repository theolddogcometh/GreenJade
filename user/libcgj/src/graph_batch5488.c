/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5488: FPS limit validity stub (Deck wave).
 *
 * Surface (unique symbols):
 *   int gj_fps_limit_ok_5488(int fps);
 *     - Return 1 if fps is 0 (unlimited / disabled) or in the closed
 *       range [15, 240] (typical desktop/Deck FPS caps), else 0.
 *   int __gj_fps_limit_ok_5488  (alias)
 *   __libcgj_batch5488_marker = "libcgj-batch5488"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_fps_limit_ok_5488 surface only; no
 * multi-def. Distinct from gj_fps_limit_ok (batch1869) and
 * gj_fps_cap_ok_u (batch2275). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5488_marker[] = "libcgj-batch5488";

#define B5488_FPS_MIN  15
#define B5488_FPS_MAX  240

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5488_fps_ok(int nFps)
{
	if (nFps == 0) {
		return 1;
	}
	if (nFps < B5488_FPS_MIN) {
		return 0;
	}
	if (nFps > B5488_FPS_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_5488 - 1 iff fps is 0 or in [15, 240].
 *
 * fps: requested frame-rate limit (signed so negatives reject cleanly).
 * 0 means unlimited/disabled and is accepted.
 * Does not call libc. No parent wires.
 */
int
gj_fps_limit_ok_5488(int nFps)
{
	(void)NULL;
	return b5488_fps_ok(nFps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fps_limit_ok_5488(int nFps)
    __attribute__((alias("gj_fps_limit_ok_5488")));
