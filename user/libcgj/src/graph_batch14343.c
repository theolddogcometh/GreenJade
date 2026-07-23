/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14343: gamescope soft bigpicture ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_14343(void);
 *     - Returns 0 (soft stub: Big Picture mode slot is soft/unprobed;
 *       not a runtime Steam Big Picture session probe).
 *   uint32_t __gj_bigpicture_ok_u_14343  (alias)
 *   __libcgj_batch14343_marker = "libcgj-batch14343"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_bigpicture_ok_u_14343 surface only; no
 * multi-def. Distinct from gj_bigpicture_ok_u_13443 (batch13443), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No Big Picture implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14343_marker[] = "libcgj-batch14343";

/* Soft-stub bigpicture ok value (unprobed). */
#define B14343_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14343_bigpicture_ok(void)
{
	return B14343_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_14343 - soft bigpicture ok stub.
 *
 * Always returns 0 (soft/unprobed Big Picture slot). Does not call
 * libc or probe Steam Big Picture mode. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_14343(void)
{
	(void)NULL;
	return b14343_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_14343(void)
    __attribute__((alias("gj_bigpicture_ok_u_14343")));
