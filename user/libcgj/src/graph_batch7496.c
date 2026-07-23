/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7496: libcgj product green extract (wave 7500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_7500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       7500 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_7500  (alias)
 *   __libcgj_batch7496_marker = "libcgj-batch7496"
 *
 * Milestone 7500 exclusive continuum CREATE-ONLY (7491-7500). Unique
 * gj_libcgj_green_7500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_7400 (batch7396), gj_libcgj_green_7300 (batch7296),
 * gj_libcgj_green_7200 (batch7196), and gj_shell_green_7500
 * (batch7495). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7496_marker[] = "libcgj-batch7496";

/* Product libcgj green lamp for wave 7500. */
#define B7496_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7496_green(void)
{
	return B7496_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_7500 - report product libcgj green for wave 7500.
 *
 * Always returns 1 (libcgj green/ready). Does not call libc or probe
 * soname. No parent wires.
 */
uint32_t
gj_libcgj_green_7500(void)
{
	(void)NULL;
	return b7496_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_7500(void)
    __attribute__((alias("gj_libcgj_green_7500")));
