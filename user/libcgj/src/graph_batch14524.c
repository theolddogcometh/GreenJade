/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14524: steam screenshot ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_screenshot_ok_u_14524(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam screenshot/runtime probe for the steam media soft continuum.
 *   uint32_t __gj_steam_screenshot_ok_u_14524  (alias)
 *   __libcgj_batch14524_marker = "libcgj-batch14524"
 *
 * Exclusive continuum CREATE-ONLY (14521-14530: steam media soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14530). Unique surface
 * only; no multi-def. Distinct from gj_steam_runtime_ok_u_* / gj_steam_net_ok_u_* and
 * sibling steam media soft stubs in prior waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14524_marker[] = "libcgj-batch14524";

/* Steam screenshot soft lamp: always off. */
#define B14524_STEAM  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14524_ok(void)
{
	return B14524_STEAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_screenshot_ok_u_14524 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_screenshot_ok_u_14524(void)
{
	(void)NULL;
	return b14524_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_screenshot_ok_u_14524(void)
    __attribute__((alias("gj_steam_screenshot_ok_u_14524")));
