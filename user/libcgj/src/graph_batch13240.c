/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13240: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13240(void);
 *     - Returns the compile-time graph batch number for this TU (13240).
 *   uint32_t __gj_batch_id_13240  (alias)
 *   __libcgj_batch13240_marker = "libcgj-batch13240"
 *
 * Exclusive continuum CREATE-ONLY (13231-13240: audio soft id
 * stubs — pipewire_ok_u_13231, wireplumber_ok_u_13232,
 * pulse_compat_ok_u_13233, alsa_ok_u_13234, jack_ok_u_13235,
 * audio_latency_ok_u_13236, bluetooth_audio_ok_u_13237,
 * hdmi_audio_ok_u_13238, audio_soft_ready_u_13239, batch_id_13240).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13040 (batch13040), gj_batch_id_12840
 * (batch12840), gj_batch_id_12640 (batch12640), gj_batch_id_12440
 * (batch12440), and gj_batch_id_12240 (batch12240). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13240_marker[] = "libcgj-batch13240";

#define B13240_BATCH_ID  13240u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13240_id(void)
{
	return B13240_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13240 - report this TU's graph batch number.
 *
 * Always returns 13240.
 */
uint32_t
gj_batch_id_13240(void)
{
	(void)NULL;
	return b13240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13240(void)
    __attribute__((alias("gj_batch_id_13240")));
