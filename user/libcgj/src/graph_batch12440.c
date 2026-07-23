/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12440: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12440(void);
 *     - Returns the compile-time graph batch number for this TU (12440).
 *   uint32_t __gj_batch_id_12440  (alias)
 *   __libcgj_batch12440_marker = "libcgj-batch12440"
 *
 * Exclusive continuum CREATE-ONLY (12431-12440: audio soft id
 * stubs — pipewire_ok_u_12431, wireplumber_ok_u_12432,
 * pulse_compat_ok_u_12433, alsa_ok_u_12434, jack_ok_u_12435,
 * audio_latency_ok_u_12436, bluetooth_audio_ok_u_12437,
 * hdmi_audio_ok_u_12438, audio_soft_ready_u_12439, batch_id_12440).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12440_marker[] = "libcgj-batch12440";

#define B12440_BATCH_ID  12440u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12440_id(void)
{
	return B12440_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12440 - report this TU's graph batch number.
 *
 * Always returns 12440.
 */
uint32_t
gj_batch_id_12440(void)
{
	(void)NULL;
	return b12440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12440(void)
    __attribute__((alias("gj_batch_id_12440")));
