/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5296: libcgj product green extract (wave 5300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_5300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       5300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_5300  (alias)
 *   __libcgj_batch5296_marker = "libcgj-batch5296"
 *
 * Milestone 5300 exclusive continuum CREATE-ONLY (5291-5300). Unique
 * gj_libcgj_green_5300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5200 (batch5196), gj_libcgj_green_5100 (batch5096),
 * gj_libcgj_green_5000 (batch4996), and gj_shell_green_5300
 * (batch5295). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5296_marker[] = "libcgj-batch5296";

/* Product libcgj green lamp for wave 5300. */
#define B5296_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5296_green(void)
{
	return B5296_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_5300 - report product libcgj green lamp for wave 5300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_5300(void)
{
	(void)NULL;
	return b5296_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_5300(void)
    __attribute__((alias("gj_libcgj_green_5300")));
