/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12792: libcgj product green extract (wave 12800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_12800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       12800 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_12800  (alias)
 *   __libcgj_batch12792_marker = "libcgj-batch12792"
 *
 * Milestone 12800 exclusive continuum CREATE-ONLY (12791-12800). Unique
 * gj_libcgj_green_12800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_12700 (batch12692), gj_libcgj_green_12600 (batch12592),
 * gj_libcgj_green_12500 (batch12492), and gj_shell_green_12800
 * (batch12791). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12792_marker[] = "libcgj-batch12792";

/* Product libcgj green lamp for wave 12800. */
#define B12792_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12792_green(void)
{
	return B12792_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_12800 - report product libcgj green lamp for wave 12800.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_12800(void)
{
	(void)NULL;
	return b12792_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_12800(void)
    __attribute__((alias("gj_libcgj_green_12800")));
