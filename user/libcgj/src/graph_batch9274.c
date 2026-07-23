/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9274: soft OpenAL stereo16 format id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_al_format_stereo16_id_9274(void);
 *     - Returns 0x1103 (AL_FORMAT_STEREO16 soft catalog value). Pure-data
 *       format id stub; does not set buffer format or call libopenal.
 *   uint32_t __gj_al_format_stereo16_id_9274  (alias)
 *   __libcgj_batch9274_marker = "libcgj-batch9274"
 *
 * Exclusive continuum CREATE-ONLY (9271-9280: openal soft id stubs —
 * al_source_ok_u_9271, al_buffer_ok_u_9272,
 * al_format_mono16_id_9273, al_format_stereo16_id_9274,
 * al_gain_ok_u_9275, al_pitch_ok_u_9276, al_loop_ok_u_9277,
 * alc_device_ok_u_9278, openal_ready_u_9279, batch_id_9280).
 * Unique surface only; no multi-def. Distinct from
 * gj_al_format_mono16_id_9273 (batch9273). No parent wires. No __int128.
 * No OpenAL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9274_marker[] = "libcgj-batch9274";

/* Soft OpenAL stereo16 format catalog id (AL_FORMAT_STEREO16 = 0x1103). */
#define B9274_AL_FORMAT_STEREO16  0x1103u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9274_stereo16_id(void)
{
	return B9274_AL_FORMAT_STEREO16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_al_format_stereo16_id_9274 - soft OpenAL stereo16 format catalog id.
 *
 * Always returns 0x1103. Soft continuum constant; does not set buffer
 * format or call libopenal. No parent wires.
 */
uint32_t
gj_al_format_stereo16_id_9274(void)
{
	(void)NULL;
	return b9274_stereo16_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_al_format_stereo16_id_9274(void)
    __attribute__((alias("gj_al_format_stereo16_id_9274")));
