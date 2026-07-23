/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10540: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10540(void);
 *     - Returns the compile-time graph batch number for this TU (10540).
 *   uint32_t __gj_batch_id_10540  (alias)
 *   __libcgj_batch10540_marker = "libcgj-batch10540"
 *
 * Exclusive continuum CREATE-ONLY (10531-10540: audio soft all→0 —
 * pipewire_ok_u_10531, wireplumber_ok_u_10532, pulse_compat_ok_u_10533,
 * alsa_ok_u_10534, jack_ok_u_10535, audio_latency_ok_u_10536,
 * bluetooth_audio_ok_u_10537, hdmi_audio_ok_u_10538,
 * audio_soft_ready_u_10539, batch_id_10540). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10540_marker[] = "libcgj-batch10540";

#define B10540_BATCH_ID  10540u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10540_id(void)
{
	return B10540_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10540 - report this TU's graph batch number.
 *
 * Always returns 10540. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10540(void)
{
	(void)NULL;
	return b10540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10540(void)
    __attribute__((alias("gj_batch_id_10540")));
