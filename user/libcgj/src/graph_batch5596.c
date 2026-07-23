/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5596: libcgj product green extract (wave 5600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5600 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5600  (alias)
 *   __libcgj_batch5596_marker = "libcgj-batch5596"
 *
 * Milestone 5600 exclusive continuum CREATE-ONLY (5591-5600). Unique
 * gj_libcgj_green_5600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5550 (batch5546), gj_libcgj_green_5500 (batch5496),
 * gj_libcgj_green_5450 (batch5446), and gj_shell_green_5600
 * (batch5595). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5596_marker[] = "libcgj-batch5596";

/* Product libcgj green lamp for wave 5600. */
#define B5596_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5596_green(void)
{
	return B5596_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5600 - report product libcgj green lamp for wave 5600.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5600(void)
{
	(void)NULL;
	return b5596_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5600(void)
    __attribute__((alias("gj_libcgj_green_5600")));
