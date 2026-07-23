/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13762: libinput soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ok_u_13762(void);
 *     - Returns 0 (soft stub: libinput seat/context slot is
 *       soft/unprobed; not a runtime libinput probe).
 *   uint32_t __gj_libinput_ok_u_13762  (alias)
 *   __libcgj_batch13762_marker = "libcgj-batch13762"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_libinput_ok_u_13762 surface only; no
 * multi-def. Distinct from gj_libinput_ok_u_13562 (batch13562),
 * gj_libinput_ok_u_13362 (batch13362), and sibling input soft ok_u
 * stubs in this wave. No parent wires. No __int128. No libinput
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13762_marker[] = "libcgj-batch13762";

/* Soft-stub libinput ok value (unprobed). */
#define B13762_LIBINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13762_libinput_ok(void)
{
	return B13762_LIBINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ok_u_13762 - soft libinput seat/context ok stub.
 *
 * Always returns 0 (soft/unprobed libinput slot). Does not call libc or
 * open a libinput context. No parent wires.
 */
uint32_t
gj_libinput_ok_u_13762(void)
{
	(void)NULL;
	return b13762_libinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ok_u_13762(void)
    __attribute__((alias("gj_libinput_ok_u_13762")));
