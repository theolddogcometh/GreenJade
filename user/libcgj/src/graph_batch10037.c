/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10037: PipeWire filter soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_filter_ok_u_10037(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; PipeWire filter path is intentionally not reported
 *       ok.
 *   uint32_t __gj_pw_filter_ok_u_10037  (alias)
 *   __libcgj_batch10037_marker = "libcgj-batch10037"
 *
 * Exclusive continuum CREATE-ONLY (10031-10040: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_10031, wireplumber_ok_u_10032,
 * pulse_compat_ok_u_10033, jack_compat_ok_u_10034, alsa_ucm_ok_u_10035,
 * bluez_audio_ok_u_10036, pw_filter_ok_u_10037, pw_module_ok_u_10038,
 * audio_soft_ready_u_10039, batch_id_10040). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10037_marker[] = "libcgj-batch10037";

/* Soft PipeWire filter-ok lamp: always off for this continuum. */
#define B10037_PW_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10037_pw_filter_ok(void)
{
	return B10037_PW_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_filter_ok_u_10037 - PipeWire filter soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create filters or
 * call libc. No parent wires.
 */
uint32_t
gj_pw_filter_ok_u_10037(void)
{
	(void)NULL;
	return b10037_pw_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_filter_ok_u_10037(void)
    __attribute__((alias("gj_pw_filter_ok_u_10037")));
