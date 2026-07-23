/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10292: libcgj product green extract (wave 10300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_10300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       10300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_10300  (alias)
 *   __libcgj_batch10292_marker = "libcgj-batch10292"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_libcgj_green_10300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_10200 (batch10192), gj_libcgj_green_10100 (batch10092),
 * gj_libcgj_green_10000 (batch9992), and gj_shell_green_10300
 * (batch10291). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10292_marker[] = "libcgj-batch10292";

/* Product libcgj green lamp for wave 10300. */
#define B10292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10292_green(void)
{
	return B10292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_10300 - report product libcgj green lamp for wave 10300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_10300(void)
{
	(void)NULL;
	return b10292_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_10300(void)
    __attribute__((alias("gj_libcgj_green_10300")));
