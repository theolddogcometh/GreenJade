/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8292: libcgj product green extract (wave 8300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_8300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       8300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_8300  (alias)
 *   __libcgj_batch8292_marker = "libcgj-batch8292"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_libcgj_green_8300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_8200 (batch8196), gj_libcgj_green_8100 (batch8096),
 * gj_libcgj_green_8000 (batch7996), and gj_shell_green_8300
 * (batch8291). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8292_marker[] = "libcgj-batch8292";

/* Product libcgj green lamp for wave 8300. */
#define B8292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8292_green(void)
{
	return B8292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_8300 - report product libcgj green lamp for wave 8300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_8300(void)
{
	(void)NULL;
	return b8292_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_8300(void)
    __attribute__((alias("gj_libcgj_green_8300")));
