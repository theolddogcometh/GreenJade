/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11337: PipeWire filter soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_filter_ok_u_11337(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; PipeWire filter path is intentionally not reported
 *       ok.
 *   uint32_t __gj_pw_filter_ok_u_11337  (alias)
 *   __libcgj_batch11337_marker = "libcgj-batch11337"
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

const char __libcgj_batch11337_marker[] = "libcgj-batch11337";

/* Soft PipeWire filter-ok lamp: always off for this continuum. */
#define B11337_PW_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11337_pw_filter_ok(void)
{
	return B11337_PW_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_filter_ok_u_11337 - PipeWire filter soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create filters or
 * call libc. No parent wires.
 */
uint32_t
gj_pw_filter_ok_u_11337(void)
{
	(void)NULL;
	return b11337_pw_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_filter_ok_u_11337(void)
    __attribute__((alias("gj_pw_filter_ok_u_11337")));
