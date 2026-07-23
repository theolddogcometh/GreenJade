/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12640: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12640(void);
 *     - Returns the compile-time graph batch number for this TU (12640).
 *   uint32_t __gj_batch_id_12640  (alias)
 *   __libcgj_batch12640_marker = "libcgj-batch12640"
 *
 * Exclusive continuum CREATE-ONLY (12631-12640: audio soft id
 * stubs — pipewire_ok_u_12631, wireplumber_ok_u_12632,
 * pulse_compat_ok_u_12633, alsa_ok_u_12634, jack_ok_u_12635,
 * audio_latency_ok_u_12636, bluetooth_audio_ok_u_12637,
 * hdmi_audio_ok_u_12638, audio_soft_ready_u_12639, batch_id_12640).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12440 (batch12440), gj_batch_id_12240
 * (batch12240), and gj_batch_id_12040 (batch12040). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12640_marker[] = "libcgj-batch12640";

#define B12640_BATCH_ID  12640u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12640_id(void)
{
	return B12640_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12640 - report this TU's graph batch number.
 *
 * Always returns 12640.
 */
uint32_t
gj_batch_id_12640(void)
{
	(void)NULL;
	return b12640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12640(void)
    __attribute__((alias("gj_batch_id_12640")));
