/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5544: browser overlay attach ready (wave 5550).
 *
 * Surface (unique symbols):
 *   uint32_t gj_browser_overlay_ready_5544(void);
 *     - Returns 1 (browser overlay attach surface is ready for the
 *       CEF/steamwebhelper continuum). Soft compile-time product tag;
 *       not a compositor or window-system probe.
 *   uint32_t __gj_browser_overlay_ready_5544  (alias)
 *   __libcgj_batch5544_marker = "libcgj-batch5544"
 *
 * Exclusive continuum CREATE-ONLY (5541-5550: CEF/steamwebhelper stubs,
 * browser overlay, store page readiness integers). Unique
 * gj_browser_overlay_ready_5544 surface only; no multi-def. Distinct
 * from gj_browser_overlay_layer_5545 (batch5545 z-order). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5544_marker[] = "libcgj-batch5544";

/* Browser overlay attach ready lamp for wave 5550. */
#define B5544_OVERLAY_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5544_ready(void)
{
	return B5544_OVERLAY_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_browser_overlay_ready_5544 - report browser overlay attach ready.
 *
 * Always returns 1 (overlay attach surface ready). Soft pure-data only;
 * does not create windows or layers. Does not call libc. No parent wires.
 */
uint32_t
gj_browser_overlay_ready_5544(void)
{
	(void)NULL;
	return b5544_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_browser_overlay_ready_5544(void)
    __attribute__((alias("gj_browser_overlay_ready_5544")));
