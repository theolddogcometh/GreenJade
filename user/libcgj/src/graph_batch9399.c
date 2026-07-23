/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9399: milestone 9400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_9400(void);
 *     - Returns the exclusive continuum wave id for the milestone 9400
 *       continuum (always 9400). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_9400  (alias)
 *   __libcgj_batch9399_marker = "libcgj-batch9399"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_continuum_wave_9400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_9400 (batch9395), gj_continuum_wave_9300
 * (batch9299), gj_continuum_wave_9200 (batch9199), and
 * gj_continuum_wave_9100 (batch9099). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9399_marker[] = "libcgj-batch9399";

/* Exclusive continuum-wave id for milestone 9400. */
#define B9399_WAVE_ID  9400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9399_wave(void)
{
	return B9399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_9400 - report the exclusive milestone-9400 wave id.
 *
 * Always returns 9400 (milestone 9400 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_9400(void)
{
	(void)NULL;
	return b9399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_9400(void)
    __attribute__((alias("gj_continuum_wave_9400")));
