/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10745: CEC soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_10745(void);
 *     - Returns 0 (soft stub: HDMI-CEC slot is soft/unprobed; not a
 *       runtime CEC probe).
 *   uint32_t __gj_cec_ok_u_10745  (alias)
 *   __libcgj_batch10745_marker = "libcgj-batch10745"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_cec_ok_u_10745 surface only; no multi-def. Distinct from
 * gj_cec_ok_u_10545 / 10345 / 10045. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10745_marker[] = "libcgj-batch10745";

/* Soft-stub CEC ok value (unprobed). */
#define B10745_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10745_cec_ok(void)
{
	return B10745_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_10745 - soft HDMI-CEC ok stub.
 *
 * Always returns 0 (soft/unprobed CEC slot). Does not call libc or
 * probe CEC hardware. No parent wires.
 */
uint32_t
gj_cec_ok_u_10745(void)
{
	(void)NULL;
	return b10745_cec_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_10745(void)
    __attribute__((alias("gj_cec_ok_u_10745")));
