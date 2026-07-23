/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14499: milestone 14500 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14500(void);
 *     - Returns the exclusive continuum wave id for the milestone 14500
 *       continuum (always 14500u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14500  (alias)
 *   __libcgj_batch14499_marker = "libcgj-batch14499"
 *
 * Milestone 14500 exclusive continuum CREATE-ONLY (14491-14500). Unique gj_continuum_wave_14500
 * surface only; no multi-def. Distinct from gj_*_14400 / gj_*_14300 / gj_*_14200 milestone
 * surfaces and sibling 14500 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14499_marker[] = "libcgj-batch14499";

/* Exclusive continuum-wave id for milestone 14500. */
#define B14499_WAVE_ID  14500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14499_wave(void)
{
	return B14499_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14500 - exclusive milestone-14500 wave id
 *
 * Always returns 14500u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_wave_14500(void)
{
	(void)NULL;
	return b14499_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14500(void)
    __attribute__((alias("gj_continuum_wave_14500")));
