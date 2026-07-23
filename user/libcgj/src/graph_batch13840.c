/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13840: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13840(void);
 *     - Returns the compile-time graph batch number for this TU (13840).
 *   uint32_t __gj_batch_id_13840  (alias)
 *   __libcgj_batch13840_marker = "libcgj-batch13840"
 *
 * Exclusive continuum CREATE-ONLY (13831-13840: audio soft id
 * stubs — pipewire_ok_u_13831, wireplumber_ok_u_13832,
 * pulse_compat_ok_u_13833, alsa_ok_u_13834, jack_ok_u_13835,
 * audio_latency_ok_u_13836, bluetooth_audio_ok_u_13837,
 * hdmi_audio_ok_u_13838, audio_soft_ready_u_13839, batch_id_13840).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13640 (batch13640), gj_batch_id_13440
 * (batch13440), gj_batch_id_13240 (batch13240), gj_batch_id_13040
 * (batch13040), gj_batch_id_12840 (batch12840), and gj_batch_id_12640
 * (batch12640). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13840_marker[] = "libcgj-batch13840";

#define B13840_BATCH_ID  13840u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13840_id(void)
{
	return B13840_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13840 - report this TU's graph batch number.
 *
 * Always returns 13840. Closes the 13831-13840 audio soft wave.
 */
uint32_t
gj_batch_id_13840(void)
{
	(void)NULL;
	return b13840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13840(void)
    __attribute__((alias("gj_batch_id_13840")));
