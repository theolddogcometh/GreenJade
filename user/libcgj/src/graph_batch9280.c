/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9280: openal soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9280(void);
 *     - Returns the compile-time graph batch number for this TU (9280).
 *   uint32_t __gj_batch_id_9280  (alias)
 *   __libcgj_batch9280_marker = "libcgj-batch9280"
 *
 * Exclusive continuum CREATE-ONLY (9271-9280: openal soft id stubs —
 * al_source_ok_u_9271, al_buffer_ok_u_9272,
 * al_format_mono16_id_9273, al_format_stereo16_id_9274,
 * al_gain_ok_u_9275, al_pitch_ok_u_9276, al_loop_ok_u_9277,
 * alc_device_ok_u_9278, openal_ready_u_9279, batch_id_9280).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No OpenAL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9280_marker[] = "libcgj-batch9280";

#define B9280_BATCH_ID  9280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9280_id(void)
{
	return B9280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9280 - report this TU's graph batch number.
 *
 * Always returns 9280. Link-time presence tags the exclusive
 * openal soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9280(void)
{
	(void)NULL;
	return b9280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9280(void)
    __attribute__((alias("gj_batch_id_9280")));
