/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13443: gamescope soft bigpicture ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_13443(void);
 *     - Returns 0 (soft stub: Big Picture mode slot is soft/unprobed;
 *       not a runtime Steam Big Picture session probe).
 *   uint32_t __gj_bigpicture_ok_u_13443  (alias)
 *   __libcgj_batch13443_marker = "libcgj-batch13443"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_bigpicture_ok_u_13443 surface only; no
 * multi-def. Distinct from gj_bigpicture_ok_u_10043 (batch10043), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No Big Picture implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13443_marker[] = "libcgj-batch13443";

/* Soft-stub bigpicture ok value (unprobed). */
#define B13443_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13443_bigpicture_ok(void)
{
	return B13443_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_13443 - soft bigpicture ok stub.
 *
 * Always returns 0 (soft/unprobed Big Picture slot). Does not call
 * libc or probe Steam Big Picture mode. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_13443(void)
{
	(void)NULL;
	return b13443_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_13443(void)
    __attribute__((alias("gj_bigpicture_ok_u_13443")));
