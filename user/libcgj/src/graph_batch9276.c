/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9276: soft OpenAL pitch-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_al_pitch_ok_u_9276(void);
 *     - Returns 1 (AL_PITCH soft-id continuum ok). Pure-data product
 *       tag; does not set source pitch or call libopenal.
 *   uint32_t __gj_al_pitch_ok_u_9276  (alias)
 *   __libcgj_batch9276_marker = "libcgj-batch9276"
 *
 * Exclusive continuum CREATE-ONLY (9271-9280: openal soft id stubs —
 * al_source_ok_u_9271, al_buffer_ok_u_9272,
 * al_format_mono16_id_9273, al_format_stereo16_id_9274,
 * al_gain_ok_u_9275, al_pitch_ok_u_9276, al_loop_ok_u_9277,
 * alc_device_ok_u_9278, openal_ready_u_9279, batch_id_9280).
 * Unique surface only; no multi-def. Distinct from gain ok flag
 * (batch9275). No parent wires. No __int128. No OpenAL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9276_marker[] = "libcgj-batch9276";

#define B9276_AL_PITCH_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9276_pitch_ok(void)
{
	return B9276_AL_PITCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_al_pitch_ok_u_9276 - soft OpenAL pitch soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not set source
 * pitch or call libopenal. No parent wires.
 */
uint32_t
gj_al_pitch_ok_u_9276(void)
{
	(void)NULL;
	return b9276_pitch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_al_pitch_ok_u_9276(void)
    __attribute__((alias("gj_al_pitch_ok_u_9276")));
