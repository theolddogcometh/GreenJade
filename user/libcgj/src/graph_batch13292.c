/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13292: libcgj product green extract (wave 13300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_13300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       13300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_13300  (alias)
 *   __libcgj_batch13292_marker = "libcgj-batch13292"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_libcgj_green_13300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_13200 (batch13192), gj_libcgj_green_13100 (batch13092),
 * gj_libcgj_green_13000 (batch12992), and gj_shell_green_13300
 * (batch13291). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13292_marker[] = "libcgj-batch13292";

/* Product libcgj green lamp for wave 13300. */
#define B13292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13292_green(void)
{
	return B13292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_13300 - report product libcgj green lamp for wave 13300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_13300(void)
{
	(void)NULL;
	return b13292_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_13300(void)
    __attribute__((alias("gj_libcgj_green_13300")));
