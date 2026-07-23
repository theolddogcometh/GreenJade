/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12040: pipewire/wireplumber soft continuum
 * + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12040(void);
 *     - Returns the compile-time graph batch number for this TU (12040).
 *   uint32_t __gj_batch_id_12040  (alias)
 *   __libcgj_batch12040_marker = "libcgj-batch12040"
 *
 * Exclusive continuum CREATE-ONLY (12031-12040: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_12031, wireplumber_ok_u_12032,
 * pulse_compat_ok_u_12033, jack_compat_ok_u_12034, alsa_ucm_ok_u_12035,
 * bluez_audio_ok_u_12036, pw_filter_ok_u_12037, pw_module_ok_u_12038,
 * audio_soft_ready_u_12039, batch_id_12040). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10040 (batch10040). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12040_marker[] = "libcgj-batch12040";

#define B12040_BATCH_ID  12040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12040_id(void)
{
	return B12040_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12040 - report this TU's graph batch number.
 *
 * Always returns 12040.
 */
uint32_t
gj_batch_id_12040(void)
{
	(void)NULL;
	return b12040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12040(void)
    __attribute__((alias("gj_batch_id_12040")));
