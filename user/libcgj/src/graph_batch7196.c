/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7196: libcgj product green extract (wave 7200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_7200(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       7200 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_7200  (alias)
 *   __libcgj_batch7196_marker = "libcgj-batch7196"
 *
 * Milestone 7200 exclusive continuum CREATE-ONLY (7191-7200). Unique
 * gj_libcgj_green_7200 surface only; no multi-def. Distinct from
 * gj_libcgj_green_7100 (batch7096), gj_libcgj_green_7000 (batch6996),
 * gj_libcgj_green_6000 (batch5996), and gj_shell_green_7200
 * (batch7195). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7196_marker[] = "libcgj-batch7196";

/* Product libcgj green lamp for wave 7200. */
#define B7196_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7196_green(void)
{
	return B7196_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_7200 - report product libcgj green lamp for wave 7200.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_7200(void)
{
	(void)NULL;
	return b7196_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_7200(void)
    __attribute__((alias("gj_libcgj_green_7200")));
