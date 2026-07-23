/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6296: libcgj product green extract (wave 6300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6300  (alias)
 *   __libcgj_batch6296_marker = "libcgj-batch6296"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_libcgj_green_6300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6200 (batch6196), gj_libcgj_green_6100 (batch6096),
 * gj_libcgj_green_6000 (batch5996), and gj_shell_green_6300
 * (batch6295). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6296_marker[] = "libcgj-batch6296";

/* Product libcgj green lamp for wave 6300. */
#define B6296_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6296_green(void)
{
	return B6296_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6300 - report product libcgj green lamp for wave 6300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6300(void)
{
	(void)NULL;
	return b6296_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6300(void)
    __attribute__((alias("gj_libcgj_green_6300")));
