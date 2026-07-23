/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13442: gamescope soft steamui ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_13442(void);
 *     - Returns 0 (soft stub: Steam UI slot is soft/unprobed; not a
 *       runtime Steam client or steamui process probe).
 *   uint32_t __gj_steamui_ok_u_13442  (alias)
 *   __libcgj_batch13442_marker = "libcgj-batch13442"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_steamui_ok_u_13442 surface only; no
 * multi-def. Distinct from gj_steamui_ok_u_10042 (batch10042), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13442_marker[] = "libcgj-batch13442";

/* Soft-stub steamui ok value (unprobed). */
#define B13442_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13442_steamui_ok(void)
{
	return B13442_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_13442 - soft steamui ok stub.
 *
 * Always returns 0 (soft/unprobed steamui slot). Does not call libc or
 * probe Steam UI processes. No parent wires.
 */
uint32_t
gj_steamui_ok_u_13442(void)
{
	(void)NULL;
	return b13442_steamui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_13442(void)
    __attribute__((alias("gj_steamui_ok_u_13442")));
