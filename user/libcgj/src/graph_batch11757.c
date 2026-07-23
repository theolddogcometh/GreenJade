/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11757: controller soft xpad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_ok_u_11757(void);
 *     - Returns 0 (soft stub: xpad/Xbox controller slot is soft/unprobed;
 *       not a runtime xpad module or Xbox HID probe).
 *   uint32_t __gj_xpad_ok_u_11757  (alias)
 *   __libcgj_batch11757_marker = "libcgj-batch11757"
 *
 * Exclusive continuum CREATE-ONLY (11751-11760: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11760). Unique
 * gj_xpad_ok_u_11757 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11757_marker[] = "libcgj-batch11757";

/* Soft-stub xpad ok value (unprobed). */
#define B11757_XPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11757_xpad_ok(void)
{
	return B11757_XPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_ok_u_11757 - soft xpad ok stub.
 *
 * Always returns 0 (soft/unprobed xpad slot). Does not call libc or
 * probe Xbox/xpad devices. No parent wires.
 */
uint32_t
gj_xpad_ok_u_11757(void)
{
	(void)NULL;
	return b11757_xpad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_ok_u_11757(void)
    __attribute__((alias("gj_xpad_ok_u_11757")));
