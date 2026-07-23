/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5357: bar3 Steam readiness lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_ready_u(void);
 *     - Returns 0 (bar3 Steam path not ready). Soft pure-data
 *       readiness lamp for Steam/Deck product continuum.
 *   uint32_t __gj_bar3_steam_ready_u  (alias)
 *   __libcgj_batch5357_marker = "libcgj-batch5357"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_bar3_steam_ready_u surface only; no
 * multi-def. Distinct from gj_bar3_install_ready_u (batch5356),
 * gj_bar3_ready_5350 (batch5347), and gj_bar3_ready_hint (batch2299).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5357_marker[] = "libcgj-batch5357";

/* Bar3 Steam readiness lamp (not ready). */
#define B5357_STEAM_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5357_steam_ready(void)
{
	return B5357_STEAM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_ready_u - report bar3 Steam-path readiness lamp.
 *
 * Always returns 0 (Steam path not ready). Soft pure-data only; does
 * not probe Steam clients. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_steam_ready_u(void)
{
	(void)NULL;
	return b5357_steam_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_ready_u(void)
    __attribute__((alias("gj_bar3_steam_ready_u")));
