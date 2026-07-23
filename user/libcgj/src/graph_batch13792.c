/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13792: libcgj product green extract (wave 13800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13800 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13800  (alias)
 *   __libcgj_batch13792_marker = "libcgj-batch13792"
 *
 * Milestone 13800 exclusive continuum CREATE-ONLY (13791-13800). Unique
 * gj_libcgj_green_13800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13700 (batch13692), gj_libcgj_green_13600 (batch13592),
 * gj_libcgj_green_13500 (batch13492), and gj_shell_green_13800
 * (batch13791). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13792_marker[] = "libcgj-batch13792";

/* Product libcgj green lamp for wave 13800. */
#define B13792_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13792_green(void)
{
	return B13792_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13800 - report product libcgj green lamp for wave 13800.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13800(void)
{
	(void)NULL;
	return b13792_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13800(void)
    __attribute__((alias("gj_libcgj_green_13800")));
