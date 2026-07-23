/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10545: CEC soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_10545(void);
 *     - Returns 0 (soft stub: HDMI-CEC slot is soft/unprobed; not a
 *       runtime CEC probe).
 *   uint32_t __gj_cec_ok_u_10545  (alias)
 *   __libcgj_batch10545_marker = "libcgj-batch10545"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_cec_ok_u_10545 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10545_marker[] = "libcgj-batch10545";

/* Soft-stub CEC ok value (unprobed). */
#define B10545_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10545_cec_ok(void)
{
	return B10545_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_10545 - soft HDMI-CEC ok stub.
 *
 * Always returns 0 (soft/unprobed CEC slot). Does not call libc or
 * probe CEC hardware. No parent wires.
 */
uint32_t
gj_cec_ok_u_10545(void)
{
	(void)NULL;
	return b10545_cec_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_10545(void)
    __attribute__((alias("gj_cec_ok_u_10545")));
