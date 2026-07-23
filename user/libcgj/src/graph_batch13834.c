/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13834: ALSA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ok_u_13834(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ALSA pcm/ctl device probe for the audio soft continuum.
 *   uint32_t __gj_alsa_ok_u_13834  (alias)
 *   __libcgj_batch13834_marker = "libcgj-batch13834"
 *
 * Exclusive continuum CREATE-ONLY (13831-13840: audio soft id
 * stubs — pipewire_ok_u_13831, wireplumber_ok_u_13832,
 * pulse_compat_ok_u_13833, alsa_ok_u_13834, jack_ok_u_13835,
 * audio_latency_ok_u_13836, bluetooth_audio_ok_u_13837,
 * hdmi_audio_ok_u_13838, audio_soft_ready_u_13839, batch_id_13840).
 * Unique surface only; no multi-def. Distinct from
 * gj_alsa_ok_u_13634 (batch13634), gj_alsa_ok_u_13434 (batch13434),
 * gj_alsa_ok_u_13234 (batch13234), gj_alsa_ok_u_13034 (batch13034),
 * gj_alsa_ok_u_12834 (batch12834), and gj_alsa_ok_u_12634
 * (batch12634). No parent wires. No __int128. No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13834_marker[] = "libcgj-batch13834";

/* Soft ALSA-ok lamp: always off (not a real ALSA probe). */
#define B13834_ALSA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13834_alsa_ok(void)
{
	return B13834_ALSA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ok_u_13834 - ALSA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open ALSA devices or
 * call libc. No parent wires.
 */
uint32_t
gj_alsa_ok_u_13834(void)
{
	(void)NULL;
	return b13834_alsa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ok_u_13834(void)
    __attribute__((alias("gj_alsa_ok_u_13834")));
