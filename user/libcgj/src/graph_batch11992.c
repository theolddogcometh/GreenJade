/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11992: libcgj product green extract (wave 12000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_12000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       12000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_12000  (alias)
 *   __libcgj_batch11992_marker = "libcgj-batch11992"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_libcgj_green_12000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_11900 (batch11892), gj_libcgj_green_11800 (batch11792),
 * gj_libcgj_green_11700 (batch11692), and gj_shell_green_12000
 * (batch11991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11992_marker[] = "libcgj-batch11992";

/* Product libcgj green lamp for wave 12000. */
#define B11992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11992_green(void)
{
	return B11992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_12000 - report product libcgj green lamp for wave 12000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_12000(void)
{
	(void)NULL;
	return b11992_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_12000(void)
    __attribute__((alias("gj_libcgj_green_12000")));
