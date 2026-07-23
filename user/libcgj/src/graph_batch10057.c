/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10057: touchpad soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_touchpad_ok_u_10057(void);
 *     - Soft HID id lamp for controller touchpad presence (always 0 —
 *       not detected / soft-unavailable). Compile-time pure-data tag;
 *       not a live touchpad/HID contact probe.
 *   uint32_t __gj_touchpad_ok_u_10057  (alias)
 *   __libcgj_batch10057_marker = "libcgj-batch10057"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique gj_touchpad_ok_u_10057
 * surface only; no multi-def. Soft stub — does not open touchpad HID
 * nodes or sample contacts. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10057_marker[] = "libcgj-batch10057";

/* Soft unavailable / not-detected touchpad lamp. */
#define B10057_TOUCHPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10057_touchpad_ok(void)
{
	return B10057_TOUCHPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touchpad_ok_u_10057 - controller touchpad soft-id lamp.
 *
 * Always returns 0. Soft pure-data HID presence tag. Does not sample
 * touchpad contacts or call libc. No parent wires.
 */
uint32_t
gj_touchpad_ok_u_10057(void)
{
	(void)NULL;
	return b10057_touchpad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_touchpad_ok_u_10057(void)
    __attribute__((alias("gj_touchpad_ok_u_10057")));
