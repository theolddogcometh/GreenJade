/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7796: libcgj product green extract (wave 7800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_7800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       7800 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_7800  (alias)
 *   __libcgj_batch7796_marker = "libcgj-batch7796"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_libcgj_green_7800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_7700 (batch7696), gj_libcgj_green_7600 (batch7596),
 * gj_libcgj_green_7500 (batch7496), and gj_shell_green_7800
 * (batch7795). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7796_marker[] = "libcgj-batch7796";

/* Product libcgj green lamp for wave 7800. */
#define B7796_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7796_green(void)
{
	return B7796_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_7800 - report product libcgj green for wave 7800.
 *
 * Always returns 1 (libcgj green/ready). Does not call libc or probe
 * soname. No parent wires.
 */
uint32_t
gj_libcgj_green_7800(void)
{
	(void)NULL;
	return b7796_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_7800(void)
    __attribute__((alias("gj_libcgj_green_7800")));
