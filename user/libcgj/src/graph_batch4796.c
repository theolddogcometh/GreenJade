/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4796: libcgj product green extract (wave 4800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4800 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4800  (alias)
 *   __libcgj_batch4796_marker = "libcgj-batch4796"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_libcgj_green_4800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4750 (batch4746), gj_libcgj_green_4700 (batch4696),
 * gj_libcgj_green_4650 (batch4646), and gj_shell_green_4800
 * (batch4795). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4796_marker[] = "libcgj-batch4796";

/* Product libcgj green lamp for wave 4800. */
#define B4796_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4796_green(void)
{
	return B4796_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4800 - report product libcgj green lamp for wave 4800.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4800(void)
{
	(void)NULL;
	return b4796_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4800(void)
    __attribute__((alias("gj_libcgj_green_4800")));
