/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5456: Steam Deck VRR capability stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_vrr_capable_5456(void);
 *     - Returns 1 (VRR / adaptive-sync path considered capable for this
 *       continuum; Deck supports variable refresh in the 40-60 Hz band).
 *       Soft compile-time display-mode product stub; not a live DRM
 *       VRR property probe.
 *   uint32_t __gj_deck_vrr_capable_5456  (alias)
 *   __libcgj_batch5456_marker = "libcgj-batch5456"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_vrr_capable_5456 surface only;
 * no multi-def. Distinct from gj_deck_hdr_capable_5455 (batch5455) and
 * gj_deck_refresh_hz_5454 (batch5454). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5456_marker[] = "libcgj-batch5456";

/* VRR capability stub (1 = path capable for this continuum). */
#define B5456_VRR_CAPABLE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5456_vrr_capable(void)
{
	return B5456_VRR_CAPABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_vrr_capable_5456 - report Steam Deck VRR capability stub.
 *
 * Always returns 1 (VRR path capable). Soft pure-data display stub;
 * does not query DRM VRR properties. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_deck_vrr_capable_5456(void)
{
	(void)NULL;
	return b5456_vrr_capable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_vrr_capable_5456(void)
    __attribute__((alias("gj_deck_vrr_capable_5456")));
