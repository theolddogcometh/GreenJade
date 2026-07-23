/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10192: libcgj product green extract (wave 10200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10200(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10200 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10200  (alias)
 *   __libcgj_batch10192_marker = "libcgj-batch10192"
 *
 * Milestone 10200 exclusive continuum CREATE-ONLY (10191-10200). Unique
 * gj_libcgj_green_10200 surface only; no multi-def. Distinct from
 * gj_libcgj_green_10100 (batch10092), gj_libcgj_green_10000 (batch9992),
 * gj_libcgj_green_9900 (batch9892), and gj_shell_green_10200
 * (batch10191). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10192_marker[] = "libcgj-batch10192";

/* Product libcgj green lamp for wave 10200. */
#define B10192_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10192_green(void)
{
	return B10192_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10200 - report product libcgj green lamp for wave 10200.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10200(void)
{
	(void)NULL;
	return b10192_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10200(void)
    __attribute__((alias("gj_libcgj_green_10200")));
