/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8892: libcgj product green extract (wave 8900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_8900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       8900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_8900  (alias)
 *   __libcgj_batch8892_marker = "libcgj-batch8892"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_libcgj_green_8900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_8800 (batch8792), gj_libcgj_green_8700 (batch8692),
 * gj_libcgj_green_8600 (batch8592), and gj_shell_green_8900
 * (batch8891). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8892_marker[] = "libcgj-batch8892";

/* Product libcgj green lamp for wave 8900. */
#define B8892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8892_green(void)
{
	return B8892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_8900 - report product libcgj green lamp for wave 8900.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_8900(void)
{
	(void)NULL;
	return b8892_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_8900(void)
    __attribute__((alias("gj_libcgj_green_8900")));
