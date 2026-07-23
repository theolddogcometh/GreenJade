/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14342: gamescope soft steamui ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_14342(void);
 *     - Returns 0 (soft stub: Steam UI slot is soft/unprobed; not a
 *       runtime Steam client or steamui process probe).
 *   uint32_t __gj_steamui_ok_u_14342  (alias)
 *   __libcgj_batch14342_marker = "libcgj-batch14342"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_steamui_ok_u_14342 surface only; no
 * multi-def. Distinct from gj_steamui_ok_u_13442 (batch13442), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No steamui implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14342_marker[] = "libcgj-batch14342";

/* Soft-stub steamui ok value (unprobed). */
#define B14342_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14342_steamui_ok(void)
{
	return B14342_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_14342 - soft steamui ok stub.
 *
 * Always returns 0 (soft/unprobed steamui slot). Does not call libc or
 * probe Steam UI processes. No parent wires.
 */
uint32_t
gj_steamui_ok_u_14342(void)
{
	(void)NULL;
	return b14342_steamui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_14342(void)
    __attribute__((alias("gj_steamui_ok_u_14342")));
