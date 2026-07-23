/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12237: PipeWire filter soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_filter_ok_u_12237(void);
 *     - Always returns 0. Soft stub for pipewire/wireplumber soft-id
 *       continuum; PipeWire filter path is intentionally not reported
 *       ok.
 *   uint32_t __gj_pw_filter_ok_u_12237  (alias)
 *   __libcgj_batch12237_marker = "libcgj-batch12237"
 *
 * Exclusive continuum CREATE-ONLY (12231-12240: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_12231, wireplumber_ok_u_12232,
 * pulse_compat_ok_u_12233, jack_compat_ok_u_12234, alsa_ucm_ok_u_12235,
 * bluez_audio_ok_u_12236, pw_filter_ok_u_12237, pw_module_ok_u_12238,
 * audio_soft_ready_u_12239, batch_id_12240). Unique surface only; no
 * multi-def. Distinct from gj_pw_filter_ok_u_10037 (batch10037),
 * gj_pw_filter_ok_u_11937 (batch11937). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12237_marker[] = "libcgj-batch12237";

/* Soft PipeWire filter-ok lamp: always off for this continuum. */
#define B12237_PW_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12237_pw_filter_ok(void)
{
	return B12237_PW_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_filter_ok_u_12237 - PipeWire filter soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create filters or
 * call libc. No parent wires.
 */
uint32_t
gj_pw_filter_ok_u_12237(void)
{
	(void)NULL;
	return b12237_pw_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_filter_ok_u_12237(void)
    __attribute__((alias("gj_pw_filter_ok_u_12237")));
