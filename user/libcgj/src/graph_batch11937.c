/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11937: PipeWire filter soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_filter_ok_u_11937(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; PipeWire filter path is intentionally not reported
 *       ok.
 *   uint32_t __gj_pw_filter_ok_u_11937  (alias)
 *   __libcgj_batch11937_marker = "libcgj-batch11937"
 *
 * Exclusive continuum CREATE-ONLY (11931-11940: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_11931, wireplumber_ok_u_11932,
 * pulse_compat_ok_u_11933, jack_compat_ok_u_11934, alsa_ucm_ok_u_11935,
 * bluez_audio_ok_u_11936, pw_filter_ok_u_11937, pw_module_ok_u_11938,
 * audio_soft_ready_u_11939, batch_id_11940). Unique surface only; no
 * multi-def. Distinct from gj_pw_filter_ok_u_10037 (batch10037). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11937_marker[] = "libcgj-batch11937";

/* Soft PipeWire filter-ok lamp: always off for this continuum. */
#define B11937_PW_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11937_pw_filter_ok(void)
{
	return B11937_PW_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_filter_ok_u_11937 - PipeWire filter soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create filters or
 * call libc. No parent wires.
 */
uint32_t
gj_pw_filter_ok_u_11937(void)
{
	(void)NULL;
	return b11937_pw_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_filter_ok_u_11937(void)
    __attribute__((alias("gj_pw_filter_ok_u_11937")));
