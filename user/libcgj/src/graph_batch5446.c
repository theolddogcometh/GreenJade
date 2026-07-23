/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5446: Steam Deck HID gamepad B (BTN_EAST).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_gp_btn_b_5446(void);
 *     - Return Linux input code for gamepad B / BTN_EAST (0x131 == 305).
 *   uint32_t __gj_hid_gp_btn_b_5446  (alias)
 *   __libcgj_batch5446_marker = "libcgj-batch5446"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_hid_gp_btn_b_5446 surface only; no multi-def.
 * Distinct from gj_hid_gp_btn_a_5445 (batch5445). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5446_marker[] = "libcgj-batch5446";

/* Linux input BTN_EAST (gamepad B on Xbox/Deck layout). */
#define B5446_HID_GP_BTN_B  ((uint32_t)0x131u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5446_btn_b(void)
{
	return B5446_HID_GP_BTN_B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_gp_btn_b_5446 - Steam Deck HID gamepad B button code (305).
 *
 * Always returns 0x131 (BTN_EAST). Soft compile-time constant for
 * freestanding HID gamepad packing; does not open /dev/input. No parent
 * wires.
 */
uint32_t
gj_hid_gp_btn_b_5446(void)
{
	(void)NULL;
	return b5446_btn_b();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_gp_btn_b_5446(void)
    __attribute__((alias("gj_hid_gp_btn_b_5446")));
