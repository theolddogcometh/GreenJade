/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4996: libcgj product green extract (wave 5000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5000  (alias)
 *   __libcgj_batch4996_marker = "libcgj-batch4996"
 *
 * Milestone 5000 exclusive continuum CREATE-ONLY (4991-5000). Unique
 * gj_libcgj_green_5000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4900 (batch4896), gj_libcgj_green_4850 (batch4846),
 * gj_libcgj_green_4800 (batch4796), and gj_shell_green_5000
 * (batch4995). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4996_marker[] = "libcgj-batch4996";

/* Product libcgj green lamp for wave 5000. */
#define B4996_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4996_green(void)
{
	return B4996_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5000 - report product libcgj green lamp for wave 5000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5000(void)
{
	(void)NULL;
	return b4996_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5000(void)
    __attribute__((alias("gj_libcgj_green_5000")));
