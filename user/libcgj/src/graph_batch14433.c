/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14433: hda mixer mic soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_mic_name_14433(void);
 *     - Returns the soft HDA mixer mic control name tag for this
 *       continuum (always 0x4D494320, fourCC "MIC "). Not an ALSA
 *       control walk.
 *   uint32_t __gj_hda_mixer_mic_name_14433  (alias)
 *   __libcgj_batch14433_marker = "libcgj-batch14433"
 *
 * Exclusive continuum CREATE-ONLY (14431-14440: hda mixer soft
 * name tags — name fourCCs; soft_ready→0; batch_id→14440). Unique gj_hda_mixer_mic_name_14433
 * surface only; no multi-def. Distinct from audio soft stubs (pipewire/alsa ok_u) and sibling
 * hda mixer soft name tags in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14433_marker[] = "libcgj-batch14433";

/* HDA mixer mic soft name fourCC "MIC ". */
#define B14433_MIC  0x4D494320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14433_name(void)
{
	return B14433_MIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_mic_name_14433 - HDA mixer mic soft name
 *
 * Always returns 0x4D494320u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_mic_name_14433(void)
{
	(void)NULL;
	return b14433_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_mic_name_14433(void)
    __attribute__((alias("gj_hda_mixer_mic_name_14433")));
