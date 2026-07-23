/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5096: libcgj product green extract (wave 5100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5100(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5100 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5100  (alias)
 *   __libcgj_batch5096_marker = "libcgj-batch5096"
 *
 * Milestone 5100 exclusive continuum CREATE-ONLY (5091-5100). Unique
 * gj_libcgj_green_5100 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5000 (batch4996), gj_libcgj_green_4900 (batch4896),
 * gj_libcgj_green_4850 (batch4846), and gj_shell_green_5100
 * (batch5095). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5096_marker[] = "libcgj-batch5096";

/* Product libcgj green lamp for wave 5100. */
#define B5096_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5096_green(void)
{
	return B5096_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5100 - report product libcgj green lamp for wave 5100.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5100(void)
{
	(void)NULL;
	return b5096_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5100(void)
    __attribute__((alias("gj_libcgj_green_5100")));
