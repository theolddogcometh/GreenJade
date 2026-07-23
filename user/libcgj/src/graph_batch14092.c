/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14092: libcgj product green extract (wave 14100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_14100(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       14100 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_14100  (alias)
 *   __libcgj_batch14092_marker = "libcgj-batch14092"
 *
 * Milestone 14100 exclusive continuum CREATE-ONLY (14091-14100). Unique
 * gj_libcgj_green_14100 surface only; no multi-def. Distinct from
 * gj_libcgj_green_14000 (batch13992), gj_libcgj_green_13000 (batch12992),
 * gj_libcgj_green_12900 (batch12892), and gj_shell_green_14100
 * (batch14091). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14092_marker[] = "libcgj-batch14092";

/* Product libcgj green lamp for wave 14100. */
#define B14092_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14092_green(void)
{
	return B14092_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_14100 - report product libcgj green lamp for wave 14100.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_14100(void)
{
	(void)NULL;
	return b14092_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_14100(void)
    __attribute__((alias("gj_libcgj_green_14100")));
