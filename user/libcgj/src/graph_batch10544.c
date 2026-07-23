/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10544: deckui soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_10544(void);
 *     - Returns 0 (soft stub: Deck UI slot is soft/unprobed; not a
 *       runtime Deck UI probe).
 *   uint32_t __gj_deckui_ok_u_10544  (alias)
 *   __libcgj_batch10544_marker = "libcgj-batch10544"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_deckui_ok_u_10544 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10544_marker[] = "libcgj-batch10544";

/* Soft-stub deckui ok value (unprobed). */
#define B10544_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10544_deckui_ok(void)
{
	return B10544_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_10544 - soft Deck UI ok stub.
 *
 * Always returns 0 (soft/unprobed deckui slot). Does not call libc or
 * probe Deck UI. No parent wires.
 */
uint32_t
gj_deckui_ok_u_10544(void)
{
	(void)NULL;
	return b10544_deckui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_10544(void)
    __attribute__((alias("gj_deckui_ok_u_10544")));
