/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13892: libcgj product green extract (wave 13900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13900  (alias)
 *   __libcgj_batch13892_marker = "libcgj-batch13892"
 *
 * Milestone 13900 exclusive continuum CREATE-ONLY (13891-13900). Unique
 * gj_libcgj_green_13900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13800 (batch13792), gj_libcgj_green_13700 (batch13692),
 * gj_libcgj_green_13600 (batch13592), and gj_shell_green_13900
 * (batch13891). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13892_marker[] = "libcgj-batch13892";

/* Product libcgj green lamp for wave 13900. */
#define B13892_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13892_green(void)
{
	return B13892_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13900 - report product libcgj green lamp for wave 13900.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13900(void)
{
	(void)NULL;
	return b13892_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13900(void)
    __attribute__((alias("gj_libcgj_green_13900")));
