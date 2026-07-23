/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10892: libcgj product green extract (wave 10900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10900  (alias)
 *   __libcgj_batch10892_marker = "libcgj-batch10892"
 *
 * Milestone 10900 exclusive continuum CREATE-ONLY (10891-10900). Unique
 * gj_libcgj_green_10900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_10800 (batch10792), gj_libcgj_green_10700 (batch10692),
 * gj_libcgj_green_10600 (batch10592), and gj_shell_green_10900
 * (batch10891). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10892_marker[] = "libcgj-batch10892";

/* Product libcgj green lamp for wave 10900. */
#define B10892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10892_green(void)
{
	return B10892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10900 - report product libcgj green lamp for wave 10900.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10900(void)
{
	(void)NULL;
	return b10892_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10900(void)
    __attribute__((alias("gj_libcgj_green_10900")));
