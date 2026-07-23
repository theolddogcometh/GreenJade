/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11555: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11555(void);
 *     - Returns 0 (soft stub: hidraw controller path slot is
 *       soft/unprobed; not a runtime /dev/hidraw* open or ioctl probe).
 *   uint32_t __gj_hidraw_ok_u_11555  (alias)
 *   __libcgj_batch11555_marker = "libcgj-batch11555"
 *
 * Exclusive continuum CREATE-ONLY (11551-11560: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11560). Unique
 * gj_hidraw_ok_u_11555 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11555_marker[] = "libcgj-batch11555";

/* Soft-stub hidraw ok value (unprobed). */
#define B11555_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11555_hidraw_ok(void)
{
	return B11555_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11555 - soft hidraw controller path ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11555(void)
{
	(void)NULL;
	return b11555_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11555(void)
    __attribute__((alias("gj_hidraw_ok_u_11555")));
