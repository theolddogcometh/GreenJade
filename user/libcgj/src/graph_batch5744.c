/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5744: display timing vertical total.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vtotal_5744(void);
 *     - Returns stub vertical total lines (1125, 1080p60 CEA).
 *   uint32_t __gj_vtotal_5744  (alias)
 *   __libcgj_batch5744_marker = "libcgj-batch5744"
 *
 * Exclusive continuum CREATE-ONLY (5741-5750: display timings, EDID
 * checksum stub, mode set ready). Unique gj_vtotal_5744 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5744_marker[] = "libcgj-batch5744";

/* Stub VTOTAL for 1080p60 (CEA-861 style blanking). */
#define B5744_VTOTAL  1125u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5744_vtotal(void)
{
	return B5744_VTOTAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vtotal_5744 - report stub vertical total (lines + blanking).
 *
 * Always returns 1125. Integer-only display timing stub; does not
 * program a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_vtotal_5744(void)
{
	(void)NULL;
	return b5744_vtotal();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vtotal_5744(void)
    __attribute__((alias("gj_vtotal_5744")));
