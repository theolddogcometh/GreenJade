/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14183: bar3 steam checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_check_u_14183(void);
 *     - Returns 0 (soft stub: steam checklist slot is soft/unprobed;
 *       not a runtime Steam client probe).
 *   uint32_t __gj_bar3_steam_check_u_14183  (alias)
 *   __libcgj_batch14183_marker = "libcgj-batch14183"
 *
 * Exclusive continuum CREATE-ONLY (14181-14190: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_steam_check_u_14183 surface only; no
 * multi-def. Distinct from gj_bar3_steam_check_u_13083,
 * gj_bar3_steam_bit (batch2498), and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14183_marker[] = "libcgj-batch14183";

/* Soft-stub steam checklist value (unprobed). */
#define B14183_STEAM_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14183_steam_check(void)
{
	return B14183_STEAM_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_check_u_14183 - soft bar3 steam checklist stub.
 *
 * Always returns 0 (soft/unprobed steam slot). Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_bar3_steam_check_u_14183(void)
{
	(void)NULL;
	return b14183_steam_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_check_u_14183(void)
    __attribute__((alias("gj_bar3_steam_check_u_14183")));
