/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14292: libcgj product green extract (wave 14300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_14300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       14300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_14300  (alias)
 *   __libcgj_batch14292_marker = "libcgj-batch14292"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_libcgj_green_14300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_14200 (batch14192), gj_libcgj_green_14100 (batch14092),
 * gj_libcgj_green_14000 (batch13992), and gj_shell_green_14300
 * (batch14291). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14292_marker[] = "libcgj-batch14292";

/* Product libcgj green lamp for wave 14300. */
#define B14292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14292_green(void)
{
	return B14292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_14300 - report product libcgj green lamp for wave 14300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_14300(void)
{
	(void)NULL;
	return b14292_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_14300(void)
    __attribute__((alias("gj_libcgj_green_14300")));
