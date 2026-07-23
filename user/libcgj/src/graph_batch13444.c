/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13444: gamescope soft deckui ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_13444(void);
 *     - Returns 0 (soft stub: Steam Deck UI / gamescope nested UI slot
 *       is soft/unprobed; not a runtime Deck UI probe).
 *   uint32_t __gj_deckui_ok_u_13444  (alias)
 *   __libcgj_batch13444_marker = "libcgj-batch13444"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_deckui_ok_u_13444 surface only; no
 * multi-def. Distinct from gj_deckui_ok_u_10044 (batch10044), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No deckui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13444_marker[] = "libcgj-batch13444";

/* Soft-stub deckui ok value (unprobed). */
#define B13444_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13444_deckui_ok(void)
{
	return B13444_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_13444 - soft deckui ok stub.
 *
 * Always returns 0 (soft/unprobed Deck UI slot). Does not call libc or
 * probe Steam Deck UI. No parent wires.
 */
uint32_t
gj_deckui_ok_u_13444(void)
{
	(void)NULL;
	return b13444_deckui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_13444(void)
    __attribute__((alias("gj_deckui_ok_u_13444")));
