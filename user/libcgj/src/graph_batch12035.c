/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12035: ALSA UCM soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ucm_ok_u_12035(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; ALSA Use Case Manager is intentionally not reported
 *       ok.
 *   uint32_t __gj_alsa_ucm_ok_u_12035  (alias)
 *   __libcgj_batch12035_marker = "libcgj-batch12035"
 *
 * Exclusive continuum CREATE-ONLY (12031-12040: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_12031, wireplumber_ok_u_12032,
 * pulse_compat_ok_u_12033, jack_compat_ok_u_12034, alsa_ucm_ok_u_12035,
 * bluez_audio_ok_u_12036, pw_filter_ok_u_12037, pw_module_ok_u_12038,
 * audio_soft_ready_u_12039, batch_id_12040). Unique surface only; no
 * multi-def. Distinct from gj_alsa_ucm_ok_u_10035 (batch10035). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12035_marker[] = "libcgj-batch12035";

/* Soft ALSA UCM-ok lamp: always off for this continuum. */
#define B12035_ALSA_UCM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12035_alsa_ucm_ok(void)
{
	return B12035_ALSA_UCM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ucm_ok_u_12035 - ALSA UCM soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ALSA UCM or call
 * libc. No parent wires.
 */
uint32_t
gj_alsa_ucm_ok_u_12035(void)
{
	(void)NULL;
	return b12035_alsa_ucm_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ucm_ok_u_12035(void)
    __attribute__((alias("gj_alsa_ucm_ok_u_12035")));
