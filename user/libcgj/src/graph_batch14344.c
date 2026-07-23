/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14344: gamescope soft deckui ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_14344(void);
 *     - Returns 0 (soft stub: Steam Deck UI / gamescope nested UI slot
 *       is soft/unprobed; not a runtime Deck UI probe).
 *   uint32_t __gj_deckui_ok_u_14344  (alias)
 *   __libcgj_batch14344_marker = "libcgj-batch14344"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_deckui_ok_u_14344 surface only; no
 * multi-def. Distinct from gj_deckui_ok_u_13444 (batch13444), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No deckui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14344_marker[] = "libcgj-batch14344";

/* Soft-stub deckui ok value (unprobed). */
#define B14344_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14344_deckui_ok(void)
{
	return B14344_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_14344 - soft deckui ok stub.
 *
 * Always returns 0 (soft/unprobed Deck UI slot). Does not call libc or
 * probe Steam Deck UI. No parent wires.
 */
uint32_t
gj_deckui_ok_u_14344(void)
{
	(void)NULL;
	return b14344_deckui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_14344(void)
    __attribute__((alias("gj_deckui_ok_u_14344")));
