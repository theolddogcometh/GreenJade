/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11392: libcgj product green extract (wave 11400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_11400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       11400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_11400  (alias)
 *   __libcgj_batch11392_marker = "libcgj-batch11392"
 *
 * Milestone 11400 exclusive continuum CREATE-ONLY (11391-11400). Unique
 * gj_libcgj_green_11400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_11300 (batch11292), gj_libcgj_green_11200 (batch11192),
 * gj_libcgj_green_11100 (batch11092), and gj_shell_green_11400
 * (batch11391). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11392_marker[] = "libcgj-batch11392";

/* Product libcgj green lamp for wave 11400. */
#define B11392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11392_green(void)
{
	return B11392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_11400 - report product libcgj green lamp for wave 11400.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_11400(void)
{
	(void)NULL;
	return b11392_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_11400(void)
    __attribute__((alias("gj_libcgj_green_11400")));
