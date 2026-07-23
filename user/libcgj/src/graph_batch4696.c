/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4696: libcgj product green extract (wave 4700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4700(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4700 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4700  (alias)
 *   __libcgj_batch4696_marker = "libcgj-batch4696"
 *
 * Milestone 4700 exclusive continuum CREATE-ONLY (4691-4700). Unique
 * gj_libcgj_green_4700 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4650 (batch4646), gj_libcgj_green_4600 (batch4596),
 * gj_libcgj_green_4550 (batch4546), and gj_shell_green_4700
 * (batch4695). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4696_marker[] = "libcgj-batch4696";

/* Product libcgj green lamp for wave 4700. */
#define B4696_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4696_green(void)
{
	return B4696_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4700 - report product libcgj green lamp for wave 4700.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4700(void)
{
	(void)NULL;
	return b4696_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4700(void)
    __attribute__((alias("gj_libcgj_green_4700")));
