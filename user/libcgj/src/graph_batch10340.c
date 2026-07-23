/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10340: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10340(void);
 *     - Returns the compile-time graph batch number for this TU (10340).
 *   uint32_t __gj_batch_id_10340  (alias)
 *   __libcgj_batch10340_marker = "libcgj-batch10340"
 *
 * Exclusive continuum CREATE-ONLY (10331-10340: audio soft all→0
 * stubs — pipewire_ok_u_10331, wireplumber_ok_u_10332,
 * pulse_compat_ok_u_10333, alsa_ok_u_10334, jack_ok_u_10335,
 * audio_latency_ok_u_10336, bluetooth_audio_ok_u_10337,
 * hdmi_audio_ok_u_10338, audio_soft_ready_u_10339, batch_id_10340).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10340_marker[] = "libcgj-batch10340";

#define B10340_BATCH_ID  10340u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10340_id(void)
{
	return B10340_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10340 - report this TU's graph batch number.
 *
 * Always returns 10340.
 */
uint32_t
gj_batch_id_10340(void)
{
	(void)NULL;
	return b10340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10340(void)
    __attribute__((alias("gj_batch_id_10340")));
