/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14335: JACK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_jack_ok_u_14335(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       JACK server or client probe for the audio soft continuum.
 *   uint32_t __gj_jack_ok_u_14335  (alias)
 *   __libcgj_batch14335_marker = "libcgj-batch14335"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_jack_ok_u_13035 (batch13035), gj_jack_ok_u_12835 (batch12835),
 * gj_jack_ok_u_12635 (batch12635), gj_jack_ok_u_12435 (batch12435),
 * gj_jack_ok_u_10335 (batch10335), and gj_jack_ok_u_10235 (batch10235).
 * No parent wires. No __int128. No JACK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14335_marker[] = "libcgj-batch14335";

/* Soft JACK-ok lamp: always off (not a real JACK probe). */
#define B14335_JACK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14335_jack_ok(void)
{
	return B14335_JACK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jack_ok_u_14335 - JACK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open a JACK client or
 * call libc. No parent wires.
 */
uint32_t
gj_jack_ok_u_14335(void)
{
	(void)NULL;
	return b14335_jack_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jack_ok_u_14335(void)
    __attribute__((alias("gj_jack_ok_u_14335")));
