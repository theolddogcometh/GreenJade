/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12459: controller soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_12459(void);
 *     - Returns 1 (soft compile-time product tag: controller soft
 *       continuum wave 12451-12460 is present/ready; not a runtime hard
 *       probe of any gamepad/HID stack).
 *   uint32_t __gj_controller_soft_ready_u_12459  (alias)
 *   __libcgj_batch12459_marker = "libcgj-batch12459"
 *
 * Exclusive continuum CREATE-ONLY (12451-12460: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12460 —
 * controller_ok_u_12451, gamepad_ok_u_12452, joystick_ok_u_12453,
 * steam_input_ok_u_12454, hidraw_ok_u_12455, evdev_pad_ok_u_12456,
 * xpad_ok_u_12457, joydev_ok_u_12458, controller_soft_ready_u_12459,
 * batch_id_12460). Unique gj_controller_soft_ready_u_12459 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (12451-12458, all→0), gj_batch_id_12460, and prior
 * gj_controller_soft_ready_u_12259 (batch12259) /
 * gj_controller_soft_ready_u_12059 (batch12059) /
 * gj_controller_soft_ready_u_11959 (batch11959). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12459_marker[] = "libcgj-batch12459";

/* Soft controller continuum ready lamp (wave present). */
#define B12459_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12459_controller_soft_ready(void)
{
	return B12459_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_12459 - soft controller continuum ready tag.
 *
 * Always returns 1 (controller soft wave ready/present). Does not call
 * libc or hard-probe any controller slot. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_12459(void)
{
	(void)NULL;
	return b12459_controller_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_12459(void)
    __attribute__((alias("gj_controller_soft_ready_u_12459")));
