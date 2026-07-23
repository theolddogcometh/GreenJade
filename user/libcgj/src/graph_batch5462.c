/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5462: Steam offline-mode flag pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_offline_mode_pack_5462(int offline, int allow_dl);
 *     — Pack offline-mode bits: bit0 = offline!=0, bit1 = allow_dl!=0.
 *       Pure integer soft encoding for client network policy.
 *   uint32_t __gj_steam_offline_mode_pack_5462  (alias)
 *   __libcgj_batch5462_marker = "libcgj-batch5462"
 *
 * Steam network/filesystem exclusive pure-data wave (5461–5470).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5462_marker[] = "libcgj-batch5462";

#define B5462_BIT_OFFLINE   0x1u
#define B5462_BIT_ALLOW_DL  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5462_pack(int nOffline, int nAllowDl)
{
	uint32_t u;

	u = 0u;
	if (nOffline != 0) {
		u |= B5462_BIT_OFFLINE;
	}
	if (nAllowDl != 0) {
		u |= B5462_BIT_ALLOW_DL;
	}
	return u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_offline_mode_pack_5462 — pack offline / allow-download flags.
 *
 * offline:  nonzero → bit0 set (client offline mode).
 * allow_dl: nonzero → bit1 set (permit local/cache downloads offline).
 * Returns a 2-bit mask; never negative.
 */
uint32_t
gj_steam_offline_mode_pack_5462(int nOffline, int nAllowDl)
{
	(void)NULL;
	return b5462_pack(nOffline, nAllowDl);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_offline_mode_pack_5462(int nOffline, int nAllowDl)
    __attribute__((alias("gj_steam_offline_mode_pack_5462")));
