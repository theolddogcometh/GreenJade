/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5545: browser overlay layer id tag (wave 5550).
 *
 * Surface (unique symbols):
 *   uint32_t gj_browser_overlay_layer_5545(void);
 *     - Returns 0 (default / unset browser overlay layer id). Soft
 *       pure-data z-order tag for the CEF overlay continuum; not a
 *       live compositor query.
 *   uint32_t __gj_browser_overlay_layer_5545  (alias)
 *   __libcgj_batch5545_marker = "libcgj-batch5545"
 *
 * Exclusive continuum CREATE-ONLY (5541-5550: CEF/steamwebhelper stubs,
 * browser overlay, store page readiness integers). Unique
 * gj_browser_overlay_layer_5545 surface only; no multi-def. Distinct
 * from gj_browser_overlay_ready_5544 (batch5544 attach ready). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5545_marker[] = "libcgj-batch5545";

/* Default / unset browser overlay layer id. */
#define B5545_OVERLAY_LAYER  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5545_layer(void)
{
	return B5545_OVERLAY_LAYER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_browser_overlay_layer_5545 - report browser overlay layer id tag.
 *
 * Always returns 0 (default / unset). Soft pure-data only; does not
 * walk window trees. Does not call libc. No parent wires.
 */
uint32_t
gj_browser_overlay_layer_5545(void)
{
	(void)NULL;
	return b5545_layer();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_browser_overlay_layer_5545(void)
    __attribute__((alias("gj_browser_overlay_layer_5545")));
