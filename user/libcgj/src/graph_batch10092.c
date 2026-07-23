/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10092: libcgj product green extract (wave 10100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10100(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10100 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10100  (alias)
 *   __libcgj_batch10092_marker = "libcgj-batch10092"
 *
 * Milestone 10100 exclusive continuum CREATE-ONLY (10091-10100). Unique
 * gj_libcgj_green_10100 surface only; no multi-def. Distinct from
 * gj_libcgj_green_9900 (batch9892), gj_libcgj_green_9100 (batch9092),
 * gj_libcgj_green_9000 (batch8992), and gj_shell_green_10100
 * (batch10091). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10092_marker[] = "libcgj-batch10092";

/* Product libcgj green lamp for wave 10100. */
#define B10092_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10092_green(void)
{
	return B10092_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10100 - report product libcgj green lamp for wave 10100.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10100(void)
{
	(void)NULL;
	return b10092_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10100(void)
    __attribute__((alias("gj_libcgj_green_10100")));
