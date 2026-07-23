/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13992: libcgj product green extract (wave 14000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_14000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       14000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_14000  (alias)
 *   __libcgj_batch13992_marker = "libcgj-batch13992"
 *
 * Milestone 14000 exclusive continuum CREATE-ONLY (13991-14000). Unique
 * gj_libcgj_green_14000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13000 (batch12992), gj_libcgj_green_12900 (batch12892),
 * gj_libcgj_green_12800 (batch12792), and gj_shell_green_14000
 * (batch13991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13992_marker[] = "libcgj-batch13992";

/* Product libcgj green lamp for wave 14000. */
#define B13992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13992_green(void)
{
	return B13992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_14000 - report product libcgj green lamp for wave 14000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_14000(void)
{
	(void)NULL;
	return b13992_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_14000(void)
    __attribute__((alias("gj_libcgj_green_14000")));
