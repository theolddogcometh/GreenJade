/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12592: libcgj product green extract (wave 12600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_12600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       12600 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_12600  (alias)
 *   __libcgj_batch12592_marker = "libcgj-batch12592"
 *
 * Milestone 12600 exclusive continuum CREATE-ONLY (12591-12600). Unique
 * gj_libcgj_green_12600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_12500 (batch12492), gj_libcgj_green_12400 (batch12392),
 * gj_libcgj_green_12300 (batch12292), and gj_shell_green_12600
 * (batch12591). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12592_marker[] = "libcgj-batch12592";

/* Product libcgj green lamp for wave 12600. */
#define B12592_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12592_green(void)
{
	return B12592_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_12600 - report product libcgj green lamp for wave 12600.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_12600(void)
{
	(void)NULL;
	return b12592_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_12600(void)
    __attribute__((alias("gj_libcgj_green_12600")));
