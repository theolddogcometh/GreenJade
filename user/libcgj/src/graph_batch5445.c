/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5445: Steam Deck HID gamepad A (BTN_SOUTH).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_gp_btn_a_5445(void);
 *     - Return Linux input code for gamepad A / BTN_SOUTH (0x130 == 304).
 *   uint32_t __gj_hid_gp_btn_a_5445  (alias)
 *   __libcgj_batch5445_marker = "libcgj-batch5445"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_hid_gp_btn_a_5445 surface only; no multi-def.
 * Distinct from gj_hid_btn_left_u (batch5321). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5445_marker[] = "libcgj-batch5445";

/* Linux input BTN_SOUTH (gamepad A on Xbox/Deck layout). */
#define B5445_HID_GP_BTN_A  ((uint32_t)0x130u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5445_btn_a(void)
{
	return B5445_HID_GP_BTN_A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_gp_btn_a_5445 - Steam Deck HID gamepad A button code (304).
 *
 * Always returns 0x130 (BTN_SOUTH). Soft compile-time constant for
 * freestanding HID gamepad packing; does not open /dev/input. No parent
 * wires.
 */
uint32_t
gj_hid_gp_btn_a_5445(void)
{
	(void)NULL;
	return b5445_btn_a();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_gp_btn_a_5445(void)
    __attribute__((alias("gj_hid_gp_btn_a_5445")));
