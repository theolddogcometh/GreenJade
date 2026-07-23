/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12392: libcgj product green extract (wave 12400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_12400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       12400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_12400  (alias)
 *   __libcgj_batch12392_marker = "libcgj-batch12392"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_libcgj_green_12400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_12300 (batch12292), gj_libcgj_green_12200 (batch12192),
 * gj_libcgj_green_12100 (batch12092), and gj_shell_green_12400
 * (batch12391). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12392_marker[] = "libcgj-batch12392";

/* Product libcgj green lamp for wave 12400. */
#define B12392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12392_green(void)
{
	return B12392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_12400 - report product libcgj green lamp for wave 12400.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_12400(void)
{
	(void)NULL;
	return b12392_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_12400(void)
    __attribute__((alias("gj_libcgj_green_12400")));
