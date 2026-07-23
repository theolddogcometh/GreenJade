/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7296: libcgj product green extract (wave 7300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_7300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       7300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_7300  (alias)
 *   __libcgj_batch7296_marker = "libcgj-batch7296"
 *
 * Milestone 7300 exclusive continuum CREATE-ONLY (7291-7300). Unique
 * gj_libcgj_green_7300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_7200 (batch7196), gj_libcgj_green_7100 (batch7096),
 * gj_libcgj_green_7000 (batch6996), and gj_shell_green_7300
 * (batch7295). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7296_marker[] = "libcgj-batch7296";

/* Product libcgj green lamp for wave 7300. */
#define B7296_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7296_green(void)
{
	return B7296_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_7300 - report product libcgj green lamp for wave 7300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_7300(void)
{
	(void)NULL;
	return b7296_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_7300(void)
    __attribute__((alias("gj_libcgj_green_7300")));
