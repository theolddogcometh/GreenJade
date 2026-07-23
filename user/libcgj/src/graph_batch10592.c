/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10592: libcgj product green extract (wave 10600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10600 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10600  (alias)
 *   __libcgj_batch10592_marker = "libcgj-batch10592"
 *
 * Milestone 10600 exclusive continuum CREATE-ONLY (10591-10600). Unique
 * gj_libcgj_green_10600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_10500 (batch10492), gj_libcgj_green_10400 (batch10392),
 * gj_libcgj_green_10300 (batch10292), and gj_shell_green_10600
 * (batch10591). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10592_marker[] = "libcgj-batch10592";

/* Product libcgj green lamp for wave 10600. */
#define B10592_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10592_green(void)
{
	return B10592_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10600 - report product libcgj green lamp for wave 10600.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10600(void)
{
	(void)NULL;
	return b10592_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10600(void)
    __attribute__((alias("gj_libcgj_green_10600")));
