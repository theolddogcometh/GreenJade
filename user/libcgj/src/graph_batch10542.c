/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10542: steamui soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_10542(void);
 *     - Returns 0 (soft stub: Steam UI slot is soft/unprobed; not a
 *       runtime Steam UI probe).
 *   uint32_t __gj_steamui_ok_u_10542  (alias)
 *   __libcgj_batch10542_marker = "libcgj-batch10542"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_steamui_ok_u_10542 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10542_marker[] = "libcgj-batch10542";

/* Soft-stub steamui ok value (unprobed). */
#define B10542_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10542_steamui_ok(void)
{
	return B10542_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_10542 - soft Steam UI ok stub.
 *
 * Always returns 0 (soft/unprobed steamui slot). Does not call libc or
 * probe Steam UI. No parent wires.
 */
uint32_t
gj_steamui_ok_u_10542(void)
{
	(void)NULL;
	return b10542_steamui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_10542(void)
    __attribute__((alias("gj_steamui_ok_u_10542")));
