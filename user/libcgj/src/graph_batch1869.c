/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1869: FPS limit validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_fps_limit_ok(int fps);
 *     — Return 1 if fps is 0 (unlimited / disabled) or in the closed
 *       range [15, 240] (typical desktop/Deck FPS caps), else 0.
 *   int __gj_fps_limit_ok  (alias)
 *   __libcgj_batch1869_marker = "libcgj-batch1869"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1869_marker[] = "libcgj-batch1869";

#define B1869_FPS_MIN  15
#define B1869_FPS_MAX  240

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1869_fps_ok(int nFps)
{
	if (nFps == 0) {
		return 1;
	}
	if (nFps < B1869_FPS_MIN) {
		return 0;
	}
	if (nFps > B1869_FPS_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok — 1 iff fps is 0 or in [15, 240].
 *
 * fps: requested frame-rate limit (signed so negatives reject cleanly).
 * 0 means unlimited/disabled and is accepted.
 */
int
gj_fps_limit_ok(int nFps)
{
	(void)sizeof(NULL);
	return b1869_fps_ok(nFps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fps_limit_ok(int nFps)
    __attribute__((alias("gj_fps_limit_ok")));
