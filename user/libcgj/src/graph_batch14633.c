/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14633: hda mixer mic soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_mic_name_14633(void);
 *     - Returns soft fourCC-style name tag for HDA mixer mic channel
 *       (compile-time constant). Not a live ALSA/HDA probe.
 *   uint32_t __gj_hda_mixer_mic_name_14633  (alias)
 *   __libcgj_batch14633_marker = "libcgj-batch14633"
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

const char __libcgj_batch14633_marker[] = "libcgj-batch14633";

/* HDA mixer mic soft name tag. */
#define B14633_HDA_NAME  0x4D494320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14633_name(void)
{
	return B14633_HDA_NAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_mic_name_14633 - soft continuum surface
 *
 * Always returns 0x4D494320u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_mic_name_14633(void)
{
	(void)NULL;
	return b14633_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_mic_name_14633(void)
    __attribute__((alias("gj_hda_mixer_mic_name_14633")));
