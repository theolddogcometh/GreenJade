/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8792: libcgj product green extract (wave 8800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_8800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       8800 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_8800  (alias)
 *   __libcgj_batch8792_marker = "libcgj-batch8792"
 *
 * Milestone 8800 exclusive continuum CREATE-ONLY (8791-8800). Unique
 * gj_libcgj_green_8800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_8700 (batch8692), gj_libcgj_green_8600 (batch8592),
 * gj_libcgj_green_8500 (batch8492), and gj_shell_green_8800
 * (batch8791). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8792_marker[] = "libcgj-batch8792";

/* Product libcgj green lamp for wave 8800. */
#define B8792_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8792_green(void)
{
	return B8792_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_8800 - report product libcgj green lamp for wave 8800.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_8800(void)
{
	(void)NULL;
	return b8792_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_8800(void)
    __attribute__((alias("gj_libcgj_green_8800")));
