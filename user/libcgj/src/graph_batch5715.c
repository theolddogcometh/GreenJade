/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5715: Steam client path readiness lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_client_path_ready_5715(void);
 *     - Returns 1 (Steam client install-path readiness surface present).
 *       Soft compile-time lamp; does not walk steamapps or steam roots.
 *   uint32_t __gj_steam_client_path_ready_5715  (alias)
 *   __libcgj_batch5715_marker = "libcgj-batch5715"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_steam_client_path_ready_5715 surface only; no multi-def.
 * Distinct from gj_steam_path_is_compatdata (batch1865),
 * gj_steam_path_is_shadercache (batch1866), and
 * gj_steam_client_bin_ready_5714 (batch5714). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5715_marker[] = "libcgj-batch5715";

/* Steam client path readiness lamp (surface present). */
#define B5715_CLIENT_PATH_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5715_client_path_ready(void)
{
	return B5715_CLIENT_PATH_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_client_path_ready_5715 - Steam client path readiness lamp.
 *
 * Always returns 1. Soft pure-data path-ready tag for Steam client
 * continuum wiring; does not open or stat any path. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_client_path_ready_5715(void)
{
	(void)NULL;
	return b5715_client_path_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_client_path_ready_5715(void)
    __attribute__((alias("gj_steam_client_path_ready_5715")));
