/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9592: libcgj product green extract (wave 9600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_9600(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       9600 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_9600  (alias)
 *   __libcgj_batch9592_marker = "libcgj-batch9592"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_libcgj_green_9600 surface only; no multi-def. Distinct from
 * gj_libcgj_green_9500 (batch9492), gj_libcgj_green_9400 (batch9392),
 * gj_libcgj_green_9300 (batch9292), and gj_shell_green_9600
 * (batch9591). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9592_marker[] = "libcgj-batch9592";

/* Product libcgj green lamp for wave 9600. */
#define B9592_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9592_green(void)
{
	return B9592_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_9600 - report product libcgj green lamp for wave 9600.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_9600(void)
{
	(void)NULL;
	return b9592_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_9600(void)
    __attribute__((alias("gj_libcgj_green_9600")));
