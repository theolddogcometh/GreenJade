/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13592: libcgj product green extract (wave 13600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13600 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13600  (alias)
 *   __libcgj_batch13592_marker = "libcgj-batch13592"
 *
 * Milestone 13600 exclusive continuum CREATE-ONLY (13591-13600). Unique
 * gj_libcgj_green_13600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13500 (batch13492), gj_libcgj_green_13400 (batch13392),
 * gj_libcgj_green_13300 (batch13292), and gj_shell_green_13600
 * (batch13591). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13592_marker[] = "libcgj-batch13592";

/* Product libcgj green lamp for wave 13600. */
#define B13592_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13592_green(void)
{
	return B13592_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13600 - report product libcgj green lamp for wave 13600.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13600(void)
{
	(void)NULL;
	return b13592_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13600(void)
    __attribute__((alias("gj_libcgj_green_13600")));
