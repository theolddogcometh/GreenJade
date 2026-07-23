/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14636: hda mixer spk soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_spk_name_14636(void);
 *     - Returns soft fourCC-style name tag for HDA mixer spk channel
 *       (compile-time constant). Not a live ALSA/HDA probe.
 *   uint32_t __gj_hda_mixer_spk_name_14636  (alias)
 *   __libcgj_batch14636_marker = "libcgj-batch14636"
 *
 * Exclusive continuum CREATE-ONLY (14631-14640: hda mixer soft
 * name tags). Unique surface only; no multi-def. Distinct from sibling
 * hda mixer soft name tags in prior waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14636_marker[] = "libcgj-batch14636";

/* HDA mixer spk soft name tag. */
#define B14636_HDA_NAME  0x53504B20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14636_name(void)
{
	return B14636_HDA_NAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_spk_name_14636 - soft continuum surface
 *
 * Always returns 0x53504B20u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_spk_name_14636(void)
{
	(void)NULL;
	return b14636_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_spk_name_14636(void)
    __attribute__((alias("gj_hda_mixer_spk_name_14636")));
