/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6496: libcgj product green extract (wave 6500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6500 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6500  (alias)
 *   __libcgj_batch6496_marker = "libcgj-batch6496"
 *
 * Milestone 6500 exclusive continuum CREATE-ONLY (6491-6500). Unique
 * gj_libcgj_green_6500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6400 (batch6396), gj_libcgj_green_6300 (batch6296),
 * gj_libcgj_green_6200 (batch6196), and gj_shell_green_6500
 * (batch6495). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6496_marker[] = "libcgj-batch6496";

/* libcgj product green lamp for wave 6500. */
#define B6496_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6496_green(void)
{
	return B6496_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6500 - report product libcgj green lamp for wave 6500.
 *
 * Always returns 1 (libcgj green/ready). Soft compile-time product
 * status tag; not a runtime dlopen or soname probe. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6500(void)
{
	(void)NULL;
	return b6496_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6500(void)
    __attribute__((alias("gj_libcgj_green_6500")));
