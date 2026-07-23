/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14629: steam media soft ready aggregate lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_media_soft_ready_u_14629(void);
 *     - Returns 0 (soft stub: steam media soft aggregate is soft/unprobed).
 *   uint32_t __gj_steam_media_soft_ready_u_14629  (alias)
 *   __libcgj_batch14629_marker = "libcgj-batch14629"
 *
 * Exclusive continuum CREATE-ONLY (14621-14630: steam media soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14630). Unique surface
 * only; no multi-def. Distinct from sibling steam media soft stubs in prior waves.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14629_marker[] = "libcgj-batch14629";

/* Steam media soft ready: always off. */
#define B14629_STEAM  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14629_ok(void)
{
	return B14629_STEAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_media_soft_ready_u_14629 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_media_soft_ready_u_14629(void)
{
	(void)NULL;
	return b14629_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_media_soft_ready_u_14629(void)
    __attribute__((alias("gj_steam_media_soft_ready_u_14629")));
