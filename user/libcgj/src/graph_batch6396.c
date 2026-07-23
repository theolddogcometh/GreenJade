/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6396: libcgj product green extract (wave 6400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6400  (alias)
 *   __libcgj_batch6396_marker = "libcgj-batch6396"
 *
 * Milestone 6400 exclusive continuum CREATE-ONLY (6391-6400). Unique
 * gj_libcgj_green_6400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6300 (batch6296), gj_libcgj_green_6200 (batch6196),
 * gj_libcgj_green_6100 (batch6096), and gj_shell_green_6400
 * (batch6395). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6396_marker[] = "libcgj-batch6396";

/* Product libcgj green lamp for wave 6400. */
#define B6396_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6396_green(void)
{
	return B6396_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6400 - report product libcgj green lamp for wave 6400.
 *
 * Always returns 1 (libcgj green/ready). Soft compile-time product
 * status tag; not a runtime dlopen or soname probe. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6400(void)
{
	(void)NULL;
	return b6396_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6400(void)
    __attribute__((alias("gj_libcgj_green_6400")));
