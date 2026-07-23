/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10056: haptics soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_haptics_ok_u_10056(void);
 *     - Soft HID id lamp for controller haptics presence (always 0 —
 *       not detected / soft-unavailable). Compile-time pure-data tag;
 *       not a live force-feedback/HID actuator probe.
 *   uint32_t __gj_haptics_ok_u_10056  (alias)
 *   __libcgj_batch10056_marker = "libcgj-batch10056"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique gj_haptics_ok_u_10056
 * surface only; no multi-def. Soft stub — does not drive haptic
 * actuators or open FF nodes. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10056_marker[] = "libcgj-batch10056";

/* Soft unavailable / not-detected haptics lamp. */
#define B10056_HAPTICS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10056_haptics_ok(void)
{
	return B10056_HAPTICS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_haptics_ok_u_10056 - controller haptics soft-id lamp.
 *
 * Always returns 0. Soft pure-data HID presence tag. Does not drive
 * actuators or call libc. No parent wires.
 */
uint32_t
gj_haptics_ok_u_10056(void)
{
	(void)NULL;
	return b10056_haptics_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_haptics_ok_u_10056(void)
    __attribute__((alias("gj_haptics_ok_u_10056")));
