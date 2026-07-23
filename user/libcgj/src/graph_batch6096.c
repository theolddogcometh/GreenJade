/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6096: libcgj product green extract (wave 6100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6100(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6100 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6100  (alias)
 *   __libcgj_batch6096_marker = "libcgj-batch6096"
 *
 * Milestone 6100 exclusive continuum CREATE-ONLY (6091-6100). Unique
 * gj_libcgj_green_6100 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6000 (batch5996), gj_libcgj_green_5900 (batch5896),
 * gj_libcgj_green_5800 (batch5796), and gj_shell_green_6100
 * (batch6095). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6096_marker[] = "libcgj-batch6096";

/* Libcgj product green lamp for wave 6100. */
#define B6096_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6096_green(void)
{
	return B6096_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6100 - report libcgj product green lamp for wave 6100.
 *
 * Always returns 1 (libcgj green/ready). Soft pure-data only; not a
 * runtime dlopen or soname probe. Does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6100(void)
{
	(void)NULL;
	return b6096_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6100(void)
    __attribute__((alias("gj_libcgj_green_6100")));
