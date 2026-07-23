/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14438: hda mixer iec958 soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_iec_name_14438(void);
 *     - Returns the soft HDA mixer IEC958/S/PDIF control name tag for
 *       this continuum (always 0x49454320, fourCC "IEC "). Not an
 *       ALSA control walk.
 *   uint32_t __gj_hda_mixer_iec_name_14438  (alias)
 *   __libcgj_batch14438_marker = "libcgj-batch14438"
 *
 * Exclusive continuum CREATE-ONLY (14431-14440: hda mixer soft
 * name tags — name fourCCs; soft_ready→0; batch_id→14440). Unique gj_hda_mixer_iec_name_14438
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

const char __libcgj_batch14438_marker[] = "libcgj-batch14438";

/* HDA mixer IEC958 soft name fourCC "IEC ". */
#define B14438_IEC  0x49454320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14438_name(void)
{
	return B14438_IEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_iec_name_14438 - HDA mixer IEC soft name
 *
 * Always returns 0x49454320u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_iec_name_14438(void)
{
	(void)NULL;
	return b14438_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_iec_name_14438(void)
    __attribute__((alias("gj_hda_mixer_iec_name_14438")));
