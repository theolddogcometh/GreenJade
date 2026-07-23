/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2275: FPS cap soft-ok predicate (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_fps_cap_ok_u(uint32_t fps);
 *     - Return 1 if fps is 0 (unlimited / disabled) or in the closed
 *       range [15, 360] (covers desktop and Deck OLED 90 Hz doubled
 *       caps with headroom). Else 0.
 *   int __gj_fps_cap_ok_u  (alias)
 *   __libcgj_batch2275_marker = "libcgj-batch2275"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_fps_limit_ok (batch1869, signed, max 240) - unique gj_fps_cap_ok_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2275_marker[] = "libcgj-batch2275";

#define B2275_FPS_MIN  15u
#define B2275_FPS_MAX  360u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if fps is 0 (unlimited) or in [B2275_FPS_MIN, B2275_FPS_MAX]. */
static int
b2275_fps_ok(uint32_t uFps)
{
	if (uFps == 0u) {
		return 1;
	}
	if (uFps < B2275_FPS_MIN) {
		return 0;
	}
	if (uFps > B2275_FPS_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_cap_ok_u - soft check that fps is a plausible frame-rate cap.
 *
 * fps: requested frame-rate limit (0 = unlimited / disabled)
 *
 * Returns 1 when fps == 0 or fps is in [15, 360], else 0.
 * Does not call libc.
 */
int
gj_fps_cap_ok_u(uint32_t uFps)
{
	(void)NULL;
	return b2275_fps_ok(uFps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fps_cap_ok_u(uint32_t uFps)
    __attribute__((alias("gj_fps_cap_ok_u")));
