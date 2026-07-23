/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14492: libcgj product green extract (wave 14500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_14500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       14500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_14500  (alias)
 *   __libcgj_batch14492_marker = "libcgj-batch14492"
 *
 * Milestone 14500 exclusive continuum CREATE-ONLY (14491-14500). Unique gj_libcgj_green_14500
 * surface only; no multi-def. Distinct from gj_*_14400 / gj_*_14300 / gj_*_14200 milestone
 * surfaces and sibling 14500 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14492_marker[] = "libcgj-batch14492";

/* Libcgj green lamp for wave 14500. */
#define B14492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14492_green(void)
{
	return B14492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_14500 - product libcgj green lamp for wave 14500
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_libcgj_green_14500(void)
{
	(void)NULL;
	return b14492_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_14500(void)
    __attribute__((alias("gj_libcgj_green_14500")));
