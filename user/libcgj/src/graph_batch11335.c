/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11335: ALSA UCM soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ucm_ok_u_11335(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; ALSA Use Case Manager is intentionally not reported
 *       ok.
 *   uint32_t __gj_alsa_ucm_ok_u_11335  (alias)
 *   __libcgj_batch11335_marker = "libcgj-batch11335"
 *
 * Exclusive continuum CREATE-ONLY (11331-11340: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_11331, wireplumber_ok_u_11332,
 * pulse_compat_ok_u_11333, jack_compat_ok_u_11334, alsa_ucm_ok_u_11335,
 * bluez_audio_ok_u_11336, pw_filter_ok_u_11337, pw_module_ok_u_11338,
 * audio_soft_ready_u_11339, batch_id_11340). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11335_marker[] = "libcgj-batch11335";

/* Soft ALSA UCM-ok lamp: always off for this continuum. */
#define B11335_ALSA_UCM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11335_alsa_ucm_ok(void)
{
	return B11335_ALSA_UCM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ucm_ok_u_11335 - ALSA UCM soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open ALSA UCM or call
 * libc. No parent wires.
 */
uint32_t
gj_alsa_ucm_ok_u_11335(void)
{
	(void)NULL;
	return b11335_alsa_ucm_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ucm_ok_u_11335(void)
    __attribute__((alias("gj_alsa_ucm_ok_u_11335")));
