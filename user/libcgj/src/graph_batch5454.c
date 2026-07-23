/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5454: Steam Deck native refresh rate (Hz).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_refresh_hz_5454(void);
 *     - Returns 60 (Steam Deck LCD/OLED common native refresh in Hz).
 *       Soft compile-time display-mode product stub; not a live DRM
 *       mode probe.
 *   uint32_t __gj_deck_refresh_hz_5454  (alias)
 *   __libcgj_batch5454_marker = "libcgj-batch5454"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_refresh_hz_5454 surface only;
 * no multi-def. Distinct from gj_deck_res_w_u (batch5352) and
 * gj_deck_res_h_u (batch5353). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5454_marker[] = "libcgj-batch5454";

/* Steam Deck common native refresh rate (Hz). */
#define B5454_REFRESH_HZ  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5454_refresh_hz(void)
{
	return B5454_REFRESH_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_refresh_hz_5454 - report Steam Deck native refresh rate in Hz.
 *
 * Always returns 60. Soft pure-data display-mode stub; does not probe
 * DRM/KMS modes. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_refresh_hz_5454(void)
{
	(void)NULL;
	return b5454_refresh_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_refresh_hz_5454(void)
    __attribute__((alias("gj_deck_refresh_hz_5454")));
