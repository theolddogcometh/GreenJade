/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12240: pipewire/wireplumber soft continuum
 * + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12240(void);
 *     - Returns the compile-time graph batch number for this TU (12240).
 *   uint32_t __gj_batch_id_12240  (alias)
 *   __libcgj_batch12240_marker = "libcgj-batch12240"
 *
 * Exclusive continuum CREATE-ONLY (12231-12240: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_12231, wireplumber_ok_u_12232,
 * pulse_compat_ok_u_12233, jack_compat_ok_u_12234, alsa_ucm_ok_u_12235,
 * bluez_audio_ok_u_12236, pw_filter_ok_u_12237, pw_module_ok_u_12238,
 * audio_soft_ready_u_12239, batch_id_12240). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10040 (batch10040), gj_batch_id_11940 (batch11940).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12240_marker[] = "libcgj-batch12240";

#define B12240_BATCH_ID  12240u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12240_id(void)
{
	return B12240_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12240 - report this TU's graph batch number.
 *
 * Always returns 12240. Soft pure-data identity for the
 * pipewire/wireplumber audio soft continuum close. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_12240(void)
{
	(void)NULL;
	return b12240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12240(void)
    __attribute__((alias("gj_batch_id_12240")));
