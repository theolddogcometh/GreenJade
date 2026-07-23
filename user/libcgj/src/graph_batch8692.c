/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8692: libcgj product green extract (wave 8700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_8700(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       8700 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_8700  (alias)
 *   __libcgj_batch8692_marker = "libcgj-batch8692"
 *
 * Milestone 8700 exclusive continuum CREATE-ONLY (8691-8700). Unique
 * gj_libcgj_green_8700 surface only; no multi-def. Distinct from
 * gj_libcgj_green_8600 (batch8592), gj_libcgj_green_8500 (batch8492),
 * gj_libcgj_green_8400 (batch8392), and gj_shell_green_8700
 * (batch8691). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8692_marker[] = "libcgj-batch8692";

/* Product libcgj green lamp for wave 8700. */
#define B8692_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8692_green(void)
{
	return B8692_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_8700 - report product libcgj green lamp for wave 8700.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_8700(void)
{
	(void)NULL;
	return b8692_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_8700(void)
    __attribute__((alias("gj_libcgj_green_8700")));
