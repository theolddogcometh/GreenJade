/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13492: libcgj product green extract (wave 13500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13500 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13500  (alias)
 *   __libcgj_batch13492_marker = "libcgj-batch13492"
 *
 * Milestone 13500 exclusive continuum CREATE-ONLY (13491-13500). Unique
 * gj_libcgj_green_13500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13400 (batch13392), gj_libcgj_green_13300 (batch13292),
 * gj_libcgj_green_13200 (batch13192), and gj_shell_green_13500
 * (batch13491). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13492_marker[] = "libcgj-batch13492";

/* Product libcgj green lamp for wave 13500. */
#define B13492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13492_green(void)
{
	return B13492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13500 - report product libcgj green lamp for wave 13500.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13500(void)
{
	(void)NULL;
	return b13492_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13500(void)
    __attribute__((alias("gj_libcgj_green_13500")));
