/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9273: soft OpenAL mono16 format id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_al_format_mono16_id_9273(void);
 *     - Returns 0x1101 (AL_FORMAT_MONO16 soft catalog value). Pure-data
 *       format id stub; does not set buffer format or call libopenal.
 *   uint32_t __gj_al_format_mono16_id_9273  (alias)
 *   __libcgj_batch9273_marker = "libcgj-batch9273"
 *
 * Exclusive continuum CREATE-ONLY (9271-9280: openal soft id stubs —
 * al_source_ok_u_9271, al_buffer_ok_u_9272,
 * al_format_mono16_id_9273, al_format_stereo16_id_9274,
 * al_gain_ok_u_9275, al_pitch_ok_u_9276, al_loop_ok_u_9277,
 * alc_device_ok_u_9278, openal_ready_u_9279, batch_id_9280).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No OpenAL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9273_marker[] = "libcgj-batch9273";

/* Soft OpenAL mono16 format catalog id (AL_FORMAT_MONO16 = 0x1101). */
#define B9273_AL_FORMAT_MONO16  0x1101u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9273_mono16_id(void)
{
	return B9273_AL_FORMAT_MONO16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_al_format_mono16_id_9273 - soft OpenAL mono16 format catalog id.
 *
 * Always returns 0x1101. Soft continuum constant; does not set buffer
 * format or call libopenal. No parent wires.
 */
uint32_t
gj_al_format_mono16_id_9273(void)
{
	(void)NULL;
	return b9273_mono16_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_al_format_mono16_id_9273(void)
    __attribute__((alias("gj_al_format_mono16_id_9273")));
