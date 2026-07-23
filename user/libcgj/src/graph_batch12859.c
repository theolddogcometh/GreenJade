/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12859: controller soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_soft_ready_u_12859(void);
 *     - Returns 1 (soft compile-time product tag: controller soft
 *       continuum wave 12851-12860 is present/ready; not a runtime hard
 *       probe of any gamepad/HID stack).
 *   uint32_t __gj_controller_soft_ready_u_12859  (alias)
 *   __libcgj_batch12859_marker = "libcgj-batch12859"
 *
 * Exclusive continuum CREATE-ONLY (12851-12860: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12860 —
 * controller_ok_u_12851, gamepad_ok_u_12852, joystick_ok_u_12853,
 * steam_input_ok_u_12854, hidraw_ok_u_12855, evdev_pad_ok_u_12856,
 * xpad_ok_u_12857, joydev_ok_u_12858, controller_soft_ready_u_12859,
 * batch_id_12860). Unique gj_controller_soft_ready_u_12859 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (12851-12858, all→0), gj_batch_id_12860, and prior
 * gj_controller_soft_ready_u_11959 (batch11959) /
 * gj_controller_soft_ready_u_11759 (batch11759). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12859_marker[] = "libcgj-batch12859";

/* Soft controller continuum ready lamp (wave present). */
#define B12859_CONTROLLER_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12859_controller_soft_ready(void)
{
	return B12859_CONTROLLER_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_soft_ready_u_12859 - soft controller continuum ready tag.
 *
 * Always returns 1 (controller soft wave ready/present). Does not call
 * libc or hard-probe any controller slot. No parent wires.
 */
uint32_t
gj_controller_soft_ready_u_12859(void)
{
	(void)NULL;
	return b12859_controller_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_soft_ready_u_12859(void)
    __attribute__((alias("gj_controller_soft_ready_u_12859")));
