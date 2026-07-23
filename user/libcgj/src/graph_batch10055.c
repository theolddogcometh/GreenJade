/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10055: gyro soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gyro_ok_u_10055(void);
 *     - Soft HID/IMU id lamp for controller gyro presence (always 0 —
 *       not detected / soft-unavailable). Compile-time pure-data tag;
 *       not a live IMU/HID sensor probe.
 *   uint32_t __gj_gyro_ok_u_10055  (alias)
 *   __libcgj_batch10055_marker = "libcgj-batch10055"
 *
 * Exclusive continuum CREATE-ONLY (10051-10060: controller/HID soft id
 * stubs — steam_controller_ok, dualsense_ok, xbox_pad_ok,
 * switch_pro_ok, gyro_ok, haptics_ok, touchpad_ok, rumble_ok,
 * controller_soft_ready, batch_id_10060). Unique gj_gyro_ok_u_10055
 * surface only; no multi-def. Soft stub — does not sample IMU or open
 * HID sensor nodes. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10055_marker[] = "libcgj-batch10055";

/* Soft unavailable / not-detected gyro lamp. */
#define B10055_GYRO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10055_gyro_ok(void)
{
	return B10055_GYRO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gyro_ok_u_10055 - controller gyro soft-id lamp.
 *
 * Always returns 0. Soft pure-data IMU/HID presence tag. Does not
 * sample gyro hardware or call libc. No parent wires.
 */
uint32_t
gj_gyro_ok_u_10055(void)
{
	(void)NULL;
	return b10055_gyro_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gyro_ok_u_10055(void)
    __attribute__((alias("gj_gyro_ok_u_10055")));
