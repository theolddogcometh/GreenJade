/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10940: audio soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10940(void);
 *     - Returns the compile-time graph batch number for this TU (10940).
 *   uint32_t __gj_batch_id_10940  (alias)
 *   __libcgj_batch10940_marker = "libcgj-batch10940"
 *
 * Exclusive continuum CREATE-ONLY (10931-10940: audio soft all→0
 * stubs — pipewire_ok_u_10931, wireplumber_ok_u_10932,
 * pulse_compat_ok_u_10933, alsa_ok_u_10934, jack_ok_u_10935,
 * audio_latency_ok_u_10936, bluetooth_audio_ok_u_10937,
 * hdmi_audio_ok_u_10938, audio_soft_ready_u_10939, batch_id_10940).
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

const char __libcgj_batch10940_marker[] = "libcgj-batch10940";

#define B10940_BATCH_ID  10940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10940_id(void)
{
	return B10940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10940 - report this TU's graph batch number.
 *
 * Always returns 10940.
 */
uint32_t
gj_batch_id_10940(void)
{
	(void)NULL;
	return b10940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10940(void)
    __attribute__((alias("gj_batch_id_10940")));
