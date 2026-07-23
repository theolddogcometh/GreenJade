/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13040: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13040(void);
 *     - Returns the compile-time graph batch number for this TU (13040).
 *   uint32_t __gj_batch_id_13040  (alias)
 *   __libcgj_batch13040_marker = "libcgj-batch13040"
 *
 * Exclusive continuum CREATE-ONLY (13031-13040: audio soft id
 * stubs — pipewire_ok_u_13031, wireplumber_ok_u_13032,
 * pulse_compat_ok_u_13033, alsa_ok_u_13034, jack_ok_u_13035,
 * audio_latency_ok_u_13036, bluetooth_audio_ok_u_13037,
 * hdmi_audio_ok_u_13038, audio_soft_ready_u_13039, batch_id_13040).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12840 (batch12840), gj_batch_id_12640
 * (batch12640), gj_batch_id_12440 (batch12440), gj_batch_id_12240
 * (batch12240), and gj_batch_id_12040 (batch12040). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13040_marker[] = "libcgj-batch13040";

#define B13040_BATCH_ID  13040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13040_id(void)
{
	return B13040_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13040 - report this TU's graph batch number.
 *
 * Always returns 13040.
 */
uint32_t
gj_batch_id_13040(void)
{
	(void)NULL;
	return b13040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13040(void)
    __attribute__((alias("gj_batch_id_13040")));
