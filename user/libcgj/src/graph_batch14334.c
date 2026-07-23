/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14334: ALSA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ok_u_14334(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ALSA pcm/ctl device probe for the audio soft continuum.
 *   uint32_t __gj_alsa_ok_u_14334  (alias)
 *   __libcgj_batch14334_marker = "libcgj-batch14334"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_alsa_ok_u_13034 (batch13034), gj_alsa_ok_u_12834 (batch12834),
 * gj_alsa_ok_u_12634 (batch12634), gj_alsa_ok_u_12434 (batch12434),
 * gj_alsa_ok_u_10334 (batch10334), and gj_alsa_ok_u_10234 (batch10234).
 * No parent wires. No __int128. No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14334_marker[] = "libcgj-batch14334";

/* Soft ALSA-ok lamp: always off (not a real ALSA probe). */
#define B14334_ALSA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14334_alsa_ok(void)
{
	return B14334_ALSA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ok_u_14334 - ALSA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open ALSA devices or
 * call libc. No parent wires.
 */
uint32_t
gj_alsa_ok_u_14334(void)
{
	(void)NULL;
	return b14334_alsa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ok_u_14334(void)
    __attribute__((alias("gj_alsa_ok_u_14334")));
