/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14639: hda mixer soft ready aggregate lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hda_mixer_soft_ready_u_14639(void);
 *     - Returns 0 (soft stub: hda mixer soft aggregate is soft/unprobed).
 *   uint32_t __gj_hda_mixer_soft_ready_u_14639  (alias)
 *   __libcgj_batch14639_marker = "libcgj-batch14639"
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

const char __libcgj_batch14639_marker[] = "libcgj-batch14639";

/* HDA mixer soft ready: always off. */
#define B14639_HDA_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14639_ready(void)
{
	return B14639_HDA_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hda_mixer_soft_ready_u_14639 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_hda_mixer_soft_ready_u_14639(void)
{
	(void)NULL;
	return b14639_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hda_mixer_soft_ready_u_14639(void)
    __attribute__((alias("gj_hda_mixer_soft_ready_u_14639")));
