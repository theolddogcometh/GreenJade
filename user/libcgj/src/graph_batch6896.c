/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6896: libcgj product green extract (wave 6900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6900(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6900 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6900  (alias)
 *   __libcgj_batch6896_marker = "libcgj-batch6896"
 *
 * Milestone 6900 exclusive continuum CREATE-ONLY (6891-6900). Unique
 * gj_libcgj_green_6900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_6800 (batch6796), gj_libcgj_green_6700 (batch6696),
 * gj_libcgj_green_6600 (batch6596), and gj_shell_green_6900
 * (batch6895). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6896_marker[] = "libcgj-batch6896";

/* Product libcgj green lamp for wave 6900. */
#define B6896_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6896_green(void)
{
	return B6896_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6900 - report product libcgj green lamp for wave 6900.
 *
 * Always returns 1 (libcgj green/ready). Soft compile-time product
 * status tag; not a runtime dlopen or soname probe. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6900(void)
{
	(void)NULL;
	return b6896_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6900(void)
    __attribute__((alias("gj_libcgj_green_6900")));
