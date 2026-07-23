/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10083: bar3 steam checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_check_u_10083(void);
 *     - Returns 0 (soft stub: steam checklist slot is soft/unprobed;
 *       not a runtime Steam client probe).
 *   uint32_t __gj_bar3_steam_check_u_10083  (alias)
 *   __libcgj_batch10083_marker = "libcgj-batch10083"
 *
 * Exclusive continuum CREATE-ONLY (10081-10090: bar3 checklist soft
 * stubs). Unique gj_bar3_steam_check_u_10083 surface only; no multi-def.
 * Distinct from gj_bar3_steam_bit (batch2498),
 * gj_bar3_checklist_steam_bit_5702, and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10083_marker[] = "libcgj-batch10083";

/* Soft-stub steam checklist value (unprobed). */
#define B10083_STEAM_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10083_steam_check(void)
{
	return B10083_STEAM_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_check_u_10083 - soft bar3 steam checklist stub.
 *
 * Always returns 0 (soft/unprobed steam slot). Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_bar3_steam_check_u_10083(void)
{
	(void)NULL;
	return b10083_steam_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_check_u_10083(void)
    __attribute__((alias("gj_bar3_steam_check_u_10083")));
