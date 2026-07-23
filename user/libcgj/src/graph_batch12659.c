/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12659: controller soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_12659(void);
 *     - Returns 1 (soft compile-time product tag: controller soft
 *       continuum wave 12651-12660 is present/ready; not a runtime hard
 *       probe of any gamepad/HID stack).
 *   uint32_t __gj_controller_soft_ready_u_12659  (alias)
 *   __libcgj_batch12659_marker = "libcgj-batch12659"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_controller_soft_ready_u_12659 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (12651-12658, all→0), gj_batch_id_12660, and prior
 * gj_controller_soft_ready_u_12459 (batch12459) /
 * gj_controller_soft_ready_u_12259 (batch12259) /
 * gj_controller_soft_ready_u_12059 (batch12059). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12659_marker[] = "libcgj-batch12659";

/* Soft controller continuum ready lamp (wave present). */
#define B12659_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12659_controller_soft_ready(void)
{
	return B12659_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_12659 - soft controller continuum ready tag.
 *
 * Always returns 1 (controller soft wave ready/present). Does not call
 * libc or hard-probe any controller slot. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_12659(void)
{
	(void)NULL;
	return b12659_controller_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_12659(void)
    __attribute__((alias("gj_controller_soft_ready_u_12659")));
