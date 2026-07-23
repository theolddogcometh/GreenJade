/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10040: pipewire/wireplumber soft continuum
 * + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10040(void);
 *     - Returns the compile-time graph batch number for this TU (10040).
 *   uint32_t __gj_batch_id_10040  (alias)
 *   __libcgj_batch10040_marker = "libcgj-batch10040"
 *
 * Exclusive continuum CREATE-ONLY (10031-10040: pipewire/wireplumber
 * soft id stubs — pipewire_ok_u_10031, wireplumber_ok_u_10032,
 * pulse_compat_ok_u_10033, jack_compat_ok_u_10034, alsa_ucm_ok_u_10035,
 * bluez_audio_ok_u_10036, pw_filter_ok_u_10037, pw_module_ok_u_10038,
 * audio_soft_ready_u_10039, batch_id_10040). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10040_marker[] = "libcgj-batch10040";

#define B10040_BATCH_ID  10040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10040_id(void)
{
	return B10040_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10040 - report this TU's graph batch number.
 *
 * Always returns 10040.
 */
uint32_t
gj_batch_id_10040(void)
{
	(void)NULL;
	return b10040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10040(void)
    __attribute__((alias("gj_batch_id_10040")));
