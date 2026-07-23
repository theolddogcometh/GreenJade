/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13363: evdev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_13363(void);
 *     - Returns 0 (soft stub: Linux evdev input node slot is
 *       soft/unprobed; not a runtime /dev/input/event* probe).
 *   uint32_t __gj_evdev_ok_u_13363  (alias)
 *   __libcgj_batch13363_marker = "libcgj-batch13363"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_evdev_ok_u_13363 surface only; no multi-def.
 * Distinct from gj_evdev_ok_u_13163 (batch13163) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No evdev
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13363_marker[] = "libcgj-batch13363";

/* Soft-stub evdev ok value (unprobed). */
#define B13363_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13363_evdev_ok(void)
{
	return B13363_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_13363 - soft Linux evdev input ok stub.
 *
 * Always returns 0 (soft/unprobed evdev slot). Does not call libc or
 * open /dev/input nodes. No parent wires.
 */
uint32_t
gj_evdev_ok_u_13363(void)
{
	(void)NULL;
	return b13363_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_13363(void)
    __attribute__((alias("gj_evdev_ok_u_13363")));
