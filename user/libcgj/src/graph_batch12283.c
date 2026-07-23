/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12283: bar3 steam checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_steam_check_u_12283(void);
 *     - Returns 0 (soft stub: steam checklist slot is soft/unprobed;
 *       not a runtime Steam client probe).
 *   uint32_t __gj_bar3_steam_check_u_12283  (alias)
 *   __libcgj_batch12283_marker = "libcgj-batch12283"
 *
 * Exclusive continuum CREATE-ONLY (12281-12290: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_steam_check_u_12283 surface only; no
 * multi-def. Distinct from gj_bar3_steam_check_u_12083,
 * gj_bar3_steam_bit (batch2498), and sibling check_u soft stubs in
 * this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12283_marker[] = "libcgj-batch12283";

/* Soft-stub steam checklist value (unprobed). */
#define B12283_STEAM_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12283_steam_check(void)
{
	return B12283_STEAM_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_steam_check_u_12283 - soft bar3 steam checklist stub.
 *
 * Always returns 0 (soft/unprobed steam slot). Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_bar3_steam_check_u_12283(void)
{
	(void)NULL;
	return b12283_steam_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_steam_check_u_12283(void)
    __attribute__((alias("gj_bar3_steam_check_u_12283")));
