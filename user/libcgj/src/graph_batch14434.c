/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14434: hda mixer line soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_line_name_14434(void);
 *     - Returns the soft HDA mixer line-in control name tag for this
 *       continuum (always 0x4C494E45, fourCC "LINE"). Not an ALSA
 *       control walk.
 *   uint32_t __gj_hda_mixer_line_name_14434  (alias)
 *   __libcgj_batch14434_marker = "libcgj-batch14434"
 *
 * Exclusive continuum CREATE-ONLY (14431-14440: hda mixer soft
 * name tags — name fourCCs; soft_ready→0; batch_id→14440). Unique gj_hda_mixer_line_name_14434
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

const char __libcgj_batch14434_marker[] = "libcgj-batch14434";

/* HDA mixer line soft name fourCC "LINE". */
#define B14434_LINE  0x4C494E45u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14434_name(void)
{
	return B14434_LINE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_line_name_14434 - HDA mixer line soft name
 *
 * Always returns 0x4C494E45u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_line_name_14434(void)
{
	(void)NULL;
	return b14434_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_line_name_14434(void)
    __attribute__((alias("gj_hda_mixer_line_name_14434")));
