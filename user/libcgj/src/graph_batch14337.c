/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14337: Bluetooth audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bluetooth_audio_ok_u_14337(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Bluetooth A2DP/HFP audio probe for the audio soft continuum.
 *   uint32_t __gj_bluetooth_audio_ok_u_14337  (alias)
 *   __libcgj_batch14337_marker = "libcgj-batch14337"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_bluetooth_audio_ok_u_13037 (batch13037),
 * gj_bluetooth_audio_ok_u_12837 (batch12837),
 * gj_bluetooth_audio_ok_u_12637 (batch12637),
 * gj_bluetooth_audio_ok_u_12437 (batch12437),
 * gj_bluetooth_audio_ok_u_10337 (batch10337), and
 * gj_bluetooth_audio_ok_u_10237 (batch10237). No parent wires. No
 * __int128. No Bluetooth audio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14337_marker[] = "libcgj-batch14337";

/* Soft Bluetooth-audio-ok lamp: always off (not a real BT audio probe). */
#define B14337_BLUETOOTH_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14337_bluetooth_audio_ok(void)
{
	return B14337_BLUETOOTH_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bluetooth_audio_ok_u_14337 - Bluetooth audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open BlueZ or call
 * libc. No parent wires.
 */
uint32_t
gj_bluetooth_audio_ok_u_14337(void)
{
	(void)NULL;
	return b14337_bluetooth_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bluetooth_audio_ok_u_14337(void)
    __attribute__((alias("gj_bluetooth_audio_ok_u_14337")));
