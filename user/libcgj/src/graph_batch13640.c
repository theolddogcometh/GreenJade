/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13640: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13640(void);
 *     - Returns the compile-time graph batch number for this TU (13640).
 *   uint32_t __gj_batch_id_13640  (alias)
 *   __libcgj_batch13640_marker = "libcgj-batch13640"
 *
 * Exclusive continuum CREATE-ONLY (13631-13640: audio soft id
 * stubs — pipewire_ok_u_13631, wireplumber_ok_u_13632,
 * pulse_compat_ok_u_13633, alsa_ok_u_13634, jack_ok_u_13635,
 * audio_latency_ok_u_13636, bluetooth_audio_ok_u_13637,
 * hdmi_audio_ok_u_13638, audio_soft_ready_u_13639, batch_id_13640).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13440 (batch13440), gj_batch_id_13240
 * (batch13240), gj_batch_id_13040 (batch13040), gj_batch_id_12840
 * (batch12840), gj_batch_id_12640 (batch12640), and gj_batch_id_12440
 * (batch12440). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13640_marker[] = "libcgj-batch13640";

#define B13640_BATCH_ID  13640u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13640_id(void)
{
	return B13640_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13640 - report this TU's graph batch number.
 *
 * Always returns 13640. Closes the 13631-13640 audio soft wave.
 */
uint32_t
gj_batch_id_13640(void)
{
	(void)NULL;
	return b13640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13640(void)
    __attribute__((alias("gj_batch_id_13640")));
