/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12885: bar3 top50 checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_top50_check_u_12885(void);
 *     - Returns 0 (soft stub: top50 checklist slot is soft/unprobed;
 *       not a runtime top-50 catalog probe).
 *   uint32_t __gj_bar3_top50_check_u_12885  (alias)
 *   __libcgj_batch12885_marker = "libcgj-batch12885"
 *
 * Exclusive continuum CREATE-ONLY (12881-12890: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_top50_check_u_12885 surface only; no
 * multi-def. Distinct from gj_bar3_top50_check_u_12685,
 * gj_bar3_top50_check_u_12485, and sibling check_u soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12885_marker[] = "libcgj-batch12885";

/* Soft-stub top50 checklist value (unprobed). */
#define B12885_TOP50_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12885_top50_check(void)
{
	return B12885_TOP50_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_top50_check_u_12885 - soft bar3 top50 checklist stub.
 *
 * Always returns 0 (soft/unprobed top50 slot). Does not call libc or
 * probe catalogs. No parent wires.
 */
uint32_t
gj_bar3_top50_check_u_12885(void)
{
	(void)NULL;
	return b12885_top50_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_top50_check_u_12885(void)
    __attribute__((alias("gj_bar3_top50_check_u_12885")));
