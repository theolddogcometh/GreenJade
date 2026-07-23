/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5449: gyro full-scale millideg/s stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gyro_fs_mdps_5449(void);
 *     - Return stub gyro full-scale in millidegrees per second (2000000).
 *   uint32_t __gj_gyro_fs_mdps_5449  (alias)
 *   __libcgj_batch5449_marker = "libcgj-batch5449"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_gyro_fs_mdps_5449 surface only; no multi-def.
 * Soft stub — does not sample IMU/gyro hardware. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5449_marker[] = "libcgj-batch5449";

/*
 * Stub gyro full-scale: 2000 deg/s * 1000 = 2000000 millideg/s.
 * Integer-only scaling constant for freestanding IMU graph wiring.
 */
#define B5449_GYRO_FS_MDPS  ((uint32_t)2000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5449_gyro_fs_mdps(void)
{
	return B5449_GYRO_FS_MDPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gyro_fs_mdps_5449 - gyro full-scale stub in millidegrees/second.
 *
 * Always returns 2000000 (2000 deg/s * 1000). Soft compile-time stub;
 * does not open IMU or HID sensor nodes. No parent wires.
 */
uint32_t
gj_gyro_fs_mdps_5449(void)
{
	(void)NULL;
	return b5449_gyro_fs_mdps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gyro_fs_mdps_5449(void)
    __attribute__((alias("gj_gyro_fs_mdps_5449")));
