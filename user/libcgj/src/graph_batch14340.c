/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14340: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14340(void);
 *     - Returns the compile-time graph batch number for this TU (14340).
 *   uint32_t __gj_batch_id_14340  (alias)
 *   __libcgj_batch14340_marker = "libcgj-batch14340"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13040 (batch13040), gj_batch_id_12840
 * (batch12840), gj_batch_id_12640 (batch12640), gj_batch_id_12440
 * (batch12440), and gj_batch_id_10340 (batch10340). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14340_marker[] = "libcgj-batch14340";

#define B14340_BATCH_ID  14340u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14340_id(void)
{
	return B14340_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14340 - report this TU's graph batch number.
 *
 * Always returns 14340.
 */
uint32_t
gj_batch_id_14340(void)
{
	(void)NULL;
	return b14340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14340(void)
    __attribute__((alias("gj_batch_id_14340")));
