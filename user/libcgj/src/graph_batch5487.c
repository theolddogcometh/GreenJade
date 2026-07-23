/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5487: game overlay layer validity stub.
 *
 * Surface (unique symbols):
 *   int gj_overlay_layer_ok_5487(uint32_t layer);
 *     - Return 1 if layer is in the closed range [0, 15] (soft overlay
 *       z-order / layer index), else 0.
 *   int __gj_overlay_layer_ok_5487  (alias)
 *   __libcgj_batch5487_marker = "libcgj-batch5487"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_overlay_layer_ok_5487 surface only; no
 * multi-def. Distinct from gj_overlay_enable_5486. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5487_marker[] = "libcgj-batch5487";

#define B5487_LAYER_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5487_layer_ok(uint32_t uLayer)
{
	if (uLayer > B5487_LAYER_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_layer_ok_5487 - soft check that overlay layer is plausible.
 *
 * layer: candidate overlay z-order / layer index
 * Returns 1 when layer is in [0, 15], else 0.
 * Does not call libc. No parent wires.
 */
int
gj_overlay_layer_ok_5487(uint32_t uLayer)
{
	(void)NULL;
	return b5487_layer_ok(uLayer);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_overlay_layer_ok_5487(uint32_t uLayer)
    __attribute__((alias("gj_overlay_layer_ok_5487")));
