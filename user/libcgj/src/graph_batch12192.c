/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12192: libcgj product green extract (wave 12200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_12200(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       12200 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_12200  (alias)
 *   __libcgj_batch12192_marker = "libcgj-batch12192"
 *
 * Milestone 12200 exclusive continuum CREATE-ONLY (12191-12200). Unique
 * gj_libcgj_green_12200 surface only; no multi-def. Distinct from
 * gj_libcgj_green_12100 (batch12092), gj_libcgj_green_12000 (batch11992),
 * gj_libcgj_green_11900 (batch11892), and gj_shell_green_12200
 * (batch12191). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12192_marker[] = "libcgj-batch12192";

/* Product libcgj green lamp for wave 12200. */
#define B12192_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12192_green(void)
{
	return B12192_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_12200 - report product libcgj green lamp for wave 12200.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_12200(void)
{
	(void)NULL;
	return b12192_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_12200(void)
    __attribute__((alias("gj_libcgj_green_12200")));
