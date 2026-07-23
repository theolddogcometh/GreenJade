/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5896: libcgj product green extract (wave 5900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5900  (alias)
 *   __libcgj_batch5896_marker = "libcgj-batch5896"
 *
 * Milestone 5900 exclusive continuum CREATE-ONLY (5891-5900). Unique
 * gj_libcgj_green_5900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5800 (batch5796), gj_libcgj_green_5700 (batch5696),
 * gj_libcgj_green_5650 (batch5646), and gj_shell_green_5900
 * (batch5895). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5896_marker[] = "libcgj-batch5896";

/* Product libcgj green lamp for wave 5900. */
#define B5896_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5896_green(void)
{
	return B5896_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5900 - report product libcgj green lamp for wave 5900.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5900(void)
{
	(void)NULL;
	return b5896_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5900(void)
    __attribute__((alias("gj_libcgj_green_5900")));
