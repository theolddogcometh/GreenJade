/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11755: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11755(void);
 *     - Returns 0 (soft stub: hidraw controller slot is soft/unprobed;
 *       not a runtime /dev/hidraw* open or HID descriptor probe).
 *   uint32_t __gj_hidraw_ok_u_11755  (alias)
 *   __libcgj_batch11755_marker = "libcgj-batch11755"
 *
 * Exclusive continuum CREATE-ONLY (11751-11760: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11760). Unique
 * gj_hidraw_ok_u_11755 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11755_marker[] = "libcgj-batch11755";

/* Soft-stub hidraw ok value (unprobed). */
#define B11755_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11755_hidraw_ok(void)
{
	return B11755_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11755 - soft hidraw ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11755(void)
{
	(void)NULL;
	return b11755_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11755(void)
    __attribute__((alias("gj_hidraw_ok_u_11755")));
