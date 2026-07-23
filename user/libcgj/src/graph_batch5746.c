/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5746: display timing refresh rate (Hz).
 *
 * Surface (unique symbols):
 *   uint32_t gj_refresh_hz_5746(void);
 *     - Returns stub vertical refresh in Hz (60).
 *   uint32_t __gj_refresh_hz_5746  (alias)
 *   __libcgj_batch5746_marker = "libcgj-batch5746"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_refresh_hz_5746 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5746_marker[] = "libcgj-batch5746";

/* Stub vertical refresh rate (whole hertz). */
#define B5746_REFRESH_HZ  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5746_refresh_hz(void)
{
	return B5746_REFRESH_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_refresh_hz_5746 - report stub vertical refresh rate in Hz.
 *
 * Always returns 60. Integer-only display timing stub; does not
 * measure a real vblank period. Does not call libc. No parent wires.
 */
uint32_t
gj_refresh_hz_5746(void)
{
	(void)NULL;
	return b5746_refresh_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_refresh_hz_5746(void)
    __attribute__((alias("gj_refresh_hz_5746")));
