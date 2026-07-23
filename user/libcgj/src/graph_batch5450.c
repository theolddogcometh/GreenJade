/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5450: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5450(void);
 *     - Returns the compile-time graph batch number for this TU (5450).
 *   uint32_t __gj_batch_id_5450  (alias)
 *   __libcgj_batch5450_marker = "libcgj-batch5450"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5450_marker[] = "libcgj-batch5450";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5450_id(void)
{
	return 5450u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5450 - report this TU's graph batch number.
 *
 * Always returns 5450. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5450(void)
{
	(void)NULL;
	return b5450_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5450(void)
    __attribute__((alias("gj_batch_id_5450")));
