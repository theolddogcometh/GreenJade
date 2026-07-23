/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14425: steam broadcast ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_broadcast_ok_u_14425(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Broadcast probe.
 *   uint32_t __gj_steam_broadcast_ok_u_14425  (alias)
 *   __libcgj_batch14425_marker = "libcgj-batch14425"
 *
 * Exclusive continuum CREATE-ONLY (14421-14430: steam media soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14430). Unique gj_steam_broadcast_ok_u_14425
 * surface only; no multi-def. Distinct from gj_steam_runtime_ok_u_* / gj_steam_net_ok_u_* and
 * sibling steam media soft stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14425_marker[] = "libcgj-batch14425";

/* Steam media soft lamp: always off. */
#define B14425_STEAM_MEDIA  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14425_ok(void)
{
	return B14425_STEAM_MEDIA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_broadcast_ok_u_14425 - steam broadcast ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_broadcast_ok_u_14425(void)
{
	(void)NULL;
	return b14425_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_broadcast_ok_u_14425(void)
    __attribute__((alias("gj_steam_broadcast_ok_u_14425")));
