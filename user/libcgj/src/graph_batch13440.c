/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13440: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13440(void);
 *     - Returns the compile-time graph batch number for this TU (13440).
 *   uint32_t __gj_batch_id_13440  (alias)
 *   __libcgj_batch13440_marker = "libcgj-batch13440"
 *
 * Exclusive continuum CREATE-ONLY (13431-13440: audio soft id
 * stubs — pipewire_ok_u_13431, wireplumber_ok_u_13432,
 * pulse_compat_ok_u_13433, alsa_ok_u_13434, jack_ok_u_13435,
 * audio_latency_ok_u_13436, bluetooth_audio_ok_u_13437,
 * hdmi_audio_ok_u_13438, audio_soft_ready_u_13439, batch_id_13440).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13240 (batch13240), gj_batch_id_13040
 * (batch13040), gj_batch_id_12840 (batch12840), gj_batch_id_12640
 * (batch12640), and gj_batch_id_12440 (batch12440). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13440_marker[] = "libcgj-batch13440";

#define B13440_BATCH_ID  13440u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13440_id(void)
{
	return B13440_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13440 - report this TU's graph batch number.
 *
 * Always returns 13440. Closes the 13431-13440 audio soft wave.
 */
uint32_t
gj_batch_id_13440(void)
{
	(void)NULL;
	return b13440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13440(void)
    __attribute__((alias("gj_batch_id_13440")));
