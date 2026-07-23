/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14631: hda mixer master soft name tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_master_name_14631(void);
 *     - Returns soft fourCC-style name tag for HDA mixer master channel
 *       (compile-time constant). Not a live ALSA/HDA probe.
 *   uint32_t __gj_hda_mixer_master_name_14631  (alias)
 *   __libcgj_batch14631_marker = "libcgj-batch14631"
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

const char __libcgj_batch14631_marker[] = "libcgj-batch14631";

/* HDA mixer master soft name tag. */
#define B14631_HDA_NAME  0x4D415354u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14631_name(void)
{
	return B14631_HDA_NAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_master_name_14631 - soft continuum surface
 *
 * Always returns 0x4D415354u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_master_name_14631(void)
{
	(void)NULL;
	return b14631_name();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_master_name_14631(void)
    __attribute__((alias("gj_hda_mixer_master_name_14631")));
