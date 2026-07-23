/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10944: deckui soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_10944(void);
 *     - Returns 0 (soft stub: Deck UI slot is soft/unprobed; not a
 *       runtime Deck UI probe).
 *   uint32_t __gj_deckui_ok_u_10944  (alias)
 *   __libcgj_batch10944_marker = "libcgj-batch10944"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_deckui_ok_u_10944 surface only; no multi-def. Distinct from
 * gj_deckui_ok_u_10744 / 10544 / 10344 / 10044. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10944_marker[] = "libcgj-batch10944";

/* Soft-stub deckui ok value (unprobed). */
#define B10944_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10944_deckui_ok(void)
{
	return B10944_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_10944 - soft Deck UI ok stub.
 *
 * Always returns 0 (soft/unprobed deckui slot). Does not call libc or
 * probe Deck UI. No parent wires.
 */
uint32_t
gj_deckui_ok_u_10944(void)
{
	(void)NULL;
	return b10944_deckui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_10944(void)
    __attribute__((alias("gj_deckui_ok_u_10944")));
