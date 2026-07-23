/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7621: setrlimit resource stub root tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setrlimit_res_root_7621(void);
 *     - Returns the soft setrlimit-resource stub root layout tag for
 *       this continuum (always 0x53524C52, fourCC "SRLR"). Not a live
 *       setrlimit(2) call.
 *   uint32_t __gj_setrlimit_res_root_7621  (alias)
 *   __libcgj_batch7621_marker = "libcgj-batch7621"
 *
 * Exclusive continuum CREATE-ONLY (7621-7630: setrlimit resource stubs).
 * Unique gj_setrlimit_res_root_7621 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7621_marker[] = "libcgj-batch7621";

/* Setrlimit resource stub root fourCC "SRLR". */
#define B7621_SRL_ROOT  0x53524C52u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7621_root(void)
{
	return B7621_SRL_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setrlimit_res_root_7621 - setrlimit resource stub root.
 *
 * Always returns 0x53524C52 ("SRLR"). Soft pure-data layout tag for the
 * 7621-7630 continuum. Does not call setrlimit or libc.
 * No parent wires.
 */
uint32_t
gj_setrlimit_res_root_7621(void)
{
	(void)NULL;
	return b7621_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setrlimit_res_root_7621(void)
    __attribute__((alias("gj_setrlimit_res_root_7621")));
