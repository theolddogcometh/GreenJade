/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11557: controller soft xpad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_ok_u_11557(void);
 *     - Returns 0 (soft stub: xpad/Xbox-compatible controller slot is
 *       soft/unprobed; not a runtime xpad kernel module or USB probe).
 *   uint32_t __gj_xpad_ok_u_11557  (alias)
 *   __libcgj_batch11557_marker = "libcgj-batch11557"
 *
 * Exclusive continuum CREATE-ONLY (11551-11560: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11560). Unique
 * gj_xpad_ok_u_11557 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11557_marker[] = "libcgj-batch11557";

/* Soft-stub xpad ok value (unprobed). */
#define B11557_XPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11557_xpad_ok(void)
{
	return B11557_XPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_ok_u_11557 - soft xpad/Xbox controller ok stub.
 *
 * Always returns 0 (soft/unprobed xpad slot). Does not call libc or
 * probe xpad/USB gamepad paths. No parent wires.
 */
uint32_t
gj_xpad_ok_u_11557(void)
{
	(void)NULL;
	return b11557_xpad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_ok_u_11557(void)
    __attribute__((alias("gj_xpad_ok_u_11557")));
