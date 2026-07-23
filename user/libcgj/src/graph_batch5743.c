/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5743: display timing horizontal total.
 *
 * Surface (unique symbols):
 *   uint32_t gj_htotal_5743(void);
 *     - Returns stub horizontal total clocks (2200, 1080p60 CEA).
 *   uint32_t __gj_htotal_5743  (alias)
 *   __libcgj_batch5743_marker = "libcgj-batch5743"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_htotal_5743 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5743_marker[] = "libcgj-batch5743";

/* Stub HTOTAL for 1080p60 (CEA-861 style blanking). */
#define B5743_HTOTAL  2200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5743_htotal(void)
{
	return B5743_HTOTAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_htotal_5743 - report stub horizontal total (pixels + blanking).
 *
 * Always returns 2200. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_htotal_5743(void)
{
	(void)NULL;
	return b5743_htotal();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_htotal_5743(void)
    __attribute__((alias("gj_htotal_5743")));
