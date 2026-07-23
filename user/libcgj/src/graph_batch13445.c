/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13445: gamescope soft CEC ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_13445(void);
 *     - Returns 0 (soft stub: HDMI-CEC / gamescope CEC slot is
 *       soft/unprobed; not a runtime CEC adapter probe).
 *   uint32_t __gj_cec_ok_u_13445  (alias)
 *   __libcgj_batch13445_marker = "libcgj-batch13445"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_cec_ok_u_13445 surface only; no
 * multi-def. Distinct from gj_cec_ok_u_10045 (batch10045), and sibling
 * gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No CEC implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13445_marker[] = "libcgj-batch13445";

/* Soft-stub CEC ok value (unprobed). */
#define B13445_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13445_cec_ok(void)
{
	return B13445_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_13445 - soft CEC ok stub.
 *
 * Always returns 0 (soft/unprobed CEC slot). Does not call libc or
 * probe HDMI-CEC adapters. No parent wires.
 */
uint32_t
gj_cec_ok_u_13445(void)
{
	(void)NULL;
	return b13445_cec_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_13445(void)
    __attribute__((alias("gj_cec_ok_u_13445")));
