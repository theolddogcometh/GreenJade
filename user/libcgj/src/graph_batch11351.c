/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11351: controller soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_ok_u_11351(void);
 *     - Returns 0 (soft stub: generic controller stack slot is
 *       soft/unprobed; not a runtime gamepad/HID probe).
 *   uint32_t __gj_controller_ok_u_11351  (alias)
 *   __libcgj_batch11351_marker = "libcgj-batch11351"
 *
 * Exclusive continuum CREATE-ONLY (11351-11360: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11360). Unique
 * gj_controller_ok_u_11351 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11351_marker[] = "libcgj-batch11351";

/* Soft-stub controller ok value (unprobed). */
#define B11351_CONTROLLER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11351_controller_ok(void)
{
	return B11351_CONTROLLER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_ok_u_11351 - soft generic controller ok stub.
 *
 * Always returns 0 (soft/unprobed controller slot). Does not call libc
 * or probe gamepad/HID devices. No parent wires.
 */
uint32_t
gj_controller_ok_u_11351(void)
{
	(void)NULL;
	return b11351_controller_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_ok_u_11351(void)
    __attribute__((alias("gj_controller_ok_u_11351")));
