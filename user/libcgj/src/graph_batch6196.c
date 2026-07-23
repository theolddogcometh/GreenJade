/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6196: libcgj product green extract (wave 6200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6200(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6200 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6200  (alias)
 *   __libcgj_batch6196_marker = "libcgj-batch6196"
 *
 * Milestone 6200 exclusive continuum CREATE-ONLY (6191-6200). Unique
 * gj_libcgj_green_6200 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6000 (batch5996), gj_libcgj_green_5900 (batch5896),
 * gj_libcgj_green_5800 (batch5796), and gj_shell_green_6200
 * (batch6195). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6196_marker[] = "libcgj-batch6196";

/* Product libcgj green lamp for wave 6200. */
#define B6196_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6196_green(void)
{
	return B6196_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6200 - report product libcgj green lamp for wave 6200.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6200(void)
{
	(void)NULL;
	return b6196_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6200(void)
    __attribute__((alias("gj_libcgj_green_6200")));
