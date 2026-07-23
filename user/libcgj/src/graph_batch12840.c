/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12840: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12840(void);
 *     - Returns the compile-time graph batch number for this TU (12840).
 *   uint32_t __gj_batch_id_12840  (alias)
 *   __libcgj_batch12840_marker = "libcgj-batch12840"
 *
 * Exclusive continuum CREATE-ONLY (12831-12840: audio soft id
 * stubs — pipewire_ok_u_12831, wireplumber_ok_u_12832,
 * pulse_compat_ok_u_12833, alsa_ok_u_12834, jack_ok_u_12835,
 * audio_latency_ok_u_12836, bluetooth_audio_ok_u_12837,
 * hdmi_audio_ok_u_12838, audio_soft_ready_u_12839, batch_id_12840).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12640 (batch12640), gj_batch_id_12440
 * (batch12440), gj_batch_id_12240 (batch12240), and
 * gj_batch_id_12040 (batch12040). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12840_marker[] = "libcgj-batch12840";

#define B12840_BATCH_ID  12840u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12840_id(void)
{
	return B12840_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12840 - report this TU's graph batch number.
 *
 * Always returns 12840.
 */
uint32_t
gj_batch_id_12840(void)
{
	(void)NULL;
	return b12840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12840(void)
    __attribute__((alias("gj_batch_id_12840")));
