/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13259: controller soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_13259(void);
 *     - Returns 1 (soft compile-time product tag: controller soft
 *       continuum wave 13251-13260 is present/ready; not a runtime hard
 *       probe of any gamepad/HID stack).
 *   uint32_t __gj_controller_soft_ready_u_13259  (alias)
 *   __libcgj_batch13259_marker = "libcgj-batch13259"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_controller_soft_ready_u_13259 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (13251-13258, all→0), gj_batch_id_13260, and prior
 * gj_controller_soft_ready_u_13059 (batch13059) /
 * gj_controller_soft_ready_u_12859 (batch12859) /
 * gj_controller_soft_ready_u_12659 (batch12659). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13259_marker[] = "libcgj-batch13259";

/* Soft controller continuum ready lamp (wave present). */
#define B13259_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13259_controller_soft_ready(void)
{
	return B13259_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_13259 - soft controller continuum ready tag.
 *
 * Always returns 1 (controller soft wave ready/present). Does not call
 * libc or hard-probe any controller slot. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_13259(void)
{
	(void)NULL;
	return b13259_controller_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_13259(void)
    __attribute__((alias("gj_controller_soft_ready_u_13259")));
