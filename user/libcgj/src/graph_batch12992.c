/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12992: libcgj product green extract (wave 13000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13000  (alias)
 *   __libcgj_batch12992_marker = "libcgj-batch12992"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_libcgj_green_13000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_12900 (batch12892), gj_libcgj_green_12800 (batch12792),
 * gj_libcgj_green_12700 (batch12692), and gj_shell_green_13000
 * (batch12991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12992_marker[] = "libcgj-batch12992";

/* Product libcgj green lamp for wave 13000. */
#define B12992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12992_green(void)
{
	return B12992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13000 - report product libcgj green lamp for wave 13000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13000(void)
{
	(void)NULL;
	return b12992_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13000(void)
    __attribute__((alias("gj_libcgj_green_13000")));
