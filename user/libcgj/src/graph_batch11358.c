/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11358: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_11358(void);
 *     - Returns 0 (soft stub: legacy joydev interface slot is
 *       soft/unprobed; not a runtime /dev/input/js* or joydev probe).
 *   uint32_t __gj_joydev_ok_u_11358  (alias)
 *   __libcgj_batch11358_marker = "libcgj-batch11358"
 *
 * Exclusive continuum CREATE-ONLY (11351-11360: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11360). Unique
 * gj_joydev_ok_u_11358 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11358_marker[] = "libcgj-batch11358";

/* Soft-stub joydev ok value (unprobed). */
#define B11358_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11358_joydev_ok(void)
{
	return B11358_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_11358 - soft joydev interface ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * probe /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_11358(void)
{
	(void)NULL;
	return b11358_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_11358(void)
    __attribute__((alias("gj_joydev_ok_u_11358")));
