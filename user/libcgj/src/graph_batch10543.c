/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10543: bigpicture soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_10543(void);
 *     - Returns 0 (soft stub: Big Picture mode slot is soft/unprobed;
 *       not a runtime Big Picture probe).
 *   uint32_t __gj_bigpicture_ok_u_10543  (alias)
 *   __libcgj_batch10543_marker = "libcgj-batch10543"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_bigpicture_ok_u_10543 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10543_marker[] = "libcgj-batch10543";

/* Soft-stub bigpicture ok value (unprobed). */
#define B10543_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10543_bigpicture_ok(void)
{
	return B10543_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_10543 - soft Big Picture mode ok stub.
 *
 * Always returns 0 (soft/unprobed bigpicture slot). Does not call libc
 * or probe Big Picture. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_10543(void)
{
	(void)NULL;
	return b10543_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_10543(void)
    __attribute__((alias("gj_bigpicture_ok_u_10543")));
