/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11892: libcgj product green extract (wave 11900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_11900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       11900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_11900  (alias)
 *   __libcgj_batch11892_marker = "libcgj-batch11892"
 *
 * Milestone 11900 exclusive continuum CREATE-ONLY (11891-11900). Unique
 * gj_libcgj_green_11900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_11800 (batch11792), gj_libcgj_green_11700 (batch11692),
 * gj_libcgj_green_11600 (batch11592), and gj_shell_green_11900
 * (batch11891). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11892_marker[] = "libcgj-batch11892";

/* Product libcgj green lamp for wave 11900. */
#define B11892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11892_green(void)
{
	return B11892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_11900 - report product libcgj green lamp for wave 11900.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_11900(void)
{
	(void)NULL;
	return b11892_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_11900(void)
    __attribute__((alias("gj_libcgj_green_11900")));
