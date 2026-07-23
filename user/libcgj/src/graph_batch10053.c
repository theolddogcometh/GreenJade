/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10053: Xbox pad soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xbox_pad_ok_u_10053(void);
 *     - Soft HID id lamp for Xbox gamepad presence (always 0 —
 *       not detected / soft-unavailable). Compile-time pure-data tag;
 *       not a live USB/XInput/HID device probe.
 *   uint32_t __gj_xbox_pad_ok_u_10053  (alias)
 *   __libcgj_batch10053_marker = "libcgj-batch10053"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique
 * gj_xbox_pad_ok_u_10053 surface only; no multi-def. Soft stub — does
 * not open HID nodes or enumerate Xbox pads. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10053_marker[] = "libcgj-batch10053";

/* Soft unavailable / not-detected Xbox pad lamp. */
#define B10053_XBOX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10053_xbox_ok(void)
{
	return B10053_XBOX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xbox_pad_ok_u_10053 - Xbox pad soft-id lamp.
 *
 * Always returns 0. Soft pure-data HID presence tag. Does not probe
 * USB/HID or call libc. No parent wires.
 */
uint32_t
gj_xbox_pad_ok_u_10053(void)
{
	(void)NULL;
	return b10053_xbox_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xbox_pad_ok_u_10053(void)
    __attribute__((alias("gj_xbox_pad_ok_u_10053")));
