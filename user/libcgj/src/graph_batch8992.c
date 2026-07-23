/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8992: libcgj product green extract (wave 9000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_9000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       9000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_9000  (alias)
 *   __libcgj_batch8992_marker = "libcgj-batch8992"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_libcgj_green_9000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_8900 (batch8892), gj_libcgj_green_8800 (batch8792),
 * gj_libcgj_green_8700 (batch8692), and gj_shell_green_9000
 * (batch8991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8992_marker[] = "libcgj-batch8992";

/* Product libcgj green lamp for wave 9000. */
#define B8992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8992_green(void)
{
	return B8992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_9000 - report product libcgj green lamp for wave 9000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_9000(void)
{
	(void)NULL;
	return b8992_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_9000(void)
    __attribute__((alias("gj_libcgj_green_9000")));
