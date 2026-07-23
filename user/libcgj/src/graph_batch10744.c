/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10744: deckui soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_10744(void);
 *     - Returns 0 (soft stub: Deck UI slot is soft/unprobed; not a
 *       runtime Deck UI probe).
 *   uint32_t __gj_deckui_ok_u_10744  (alias)
 *   __libcgj_batch10744_marker = "libcgj-batch10744"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_deckui_ok_u_10744 surface only; no multi-def. Distinct from
 * gj_deckui_ok_u_10544 / 10344 / 10044. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10744_marker[] = "libcgj-batch10744";

/* Soft-stub deckui ok value (unprobed). */
#define B10744_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10744_deckui_ok(void)
{
	return B10744_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_10744 - soft Deck UI ok stub.
 *
 * Always returns 0 (soft/unprobed deckui slot). Does not call libc or
 * probe Deck UI. No parent wires.
 */
uint32_t
gj_deckui_ok_u_10744(void)
{
	(void)NULL;
	return b10744_deckui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_10744(void)
    __attribute__((alias("gj_deckui_ok_u_10744")));
