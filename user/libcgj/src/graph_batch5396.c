/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5396: libcgj product green extract (wave 5400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5400  (alias)
 *   __libcgj_batch5396_marker = "libcgj-batch5396"
 *
 * Milestone 5400 exclusive continuum CREATE-ONLY (5391-5400). Unique
 * gj_libcgj_green_5400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5350 (batch5346), gj_libcgj_green_5300 (batch5296),
 * gj_libcgj_green_5200 (batch5196), and gj_shell_green_5400
 * (batch5395). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5396_marker[] = "libcgj-batch5396";

/* Product libcgj green lamp for wave 5400. */
#define B5396_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5396_green(void)
{
	return B5396_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5400 - report product libcgj green lamp for wave 5400.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5400(void)
{
	(void)NULL;
	return b5396_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5400(void)
    __attribute__((alias("gj_libcgj_green_5400")));
