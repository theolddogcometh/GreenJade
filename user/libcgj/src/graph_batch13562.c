/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13562: libinput soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ok_u_13562(void);
 *     - Returns 0 (soft stub: libinput seat/context slot is
 *       soft/unprobed; not a runtime libinput probe).
 *   uint32_t __gj_libinput_ok_u_13562  (alias)
 *   __libcgj_batch13562_marker = "libcgj-batch13562"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_libinput_ok_u_13562 surface only; no
 * multi-def. Distinct from gj_libinput_ok_u_13362 (batch13362) and
 * sibling input soft ok_u stubs in this wave. No parent wires.
 * No __int128. No libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13562_marker[] = "libcgj-batch13562";

/* Soft-stub libinput ok value (unprobed). */
#define B13562_LIBINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13562_libinput_ok(void)
{
	return B13562_LIBINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ok_u_13562 - soft libinput seat/context ok stub.
 *
 * Always returns 0 (soft/unprobed libinput slot). Does not call libc or
 * open a libinput context. No parent wires.
 */
uint32_t
gj_libinput_ok_u_13562(void)
{
	(void)NULL;
	return b13562_libinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ok_u_13562(void)
    __attribute__((alias("gj_libinput_ok_u_13562")));
