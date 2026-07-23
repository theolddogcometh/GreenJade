/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5666: steam.sh surface ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_sh_ready_5666(void);
 *     - Returns 1 (steam.sh launch-code surface present for the
 *       exclusive continuum). Soft compile-time product tag; not a
 *       filesystem probe of steam.sh.
 *   uint32_t gj_batch_id_5666(void);
 *   uint32_t __gj_steam_sh_ready_5666 / __gj_batch_id_5666
 *   __libcgj_batch5666_marker = "libcgj-batch5666"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers). Unique *_5666
 * surfaces only; no multi-def. Distinct from
 * gj_steam_sh_launch_mode_5664 / gj_steam_sh_exit_code_5665.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5666_marker[] = "libcgj-batch5666";

#define B5666_BATCH_ID  5666u
#define B5666_SH_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5666_ready(void)
{
	return B5666_SH_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_sh_ready_5666 - steam.sh launch surface ready lamp.
 *
 * Always returns 1. Soft pure-data only; does not open or exec steam.sh.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_steam_sh_ready_5666(void)
{
	(void)NULL;
	return b5666_ready();
}

uint32_t
gj_batch_id_5666(void)
{
	return B5666_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_sh_ready_5666(void)
    __attribute__((alias("gj_steam_sh_ready_5666")));

uint32_t __gj_batch_id_5666(void)
    __attribute__((alias("gj_batch_id_5666")));
