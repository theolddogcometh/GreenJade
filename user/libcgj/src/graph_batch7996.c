/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7996: libcgj product green extract (wave 8000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_8000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       8000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_8000  (alias)
 *   __libcgj_batch7996_marker = "libcgj-batch7996"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_libcgj_green_8000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_7000 (batch6996), gj_libcgj_green_6000 (batch5996),
 * gj_libcgj_green_5900 (batch5896), and gj_shell_green_8000
 * (batch7995). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7996_marker[] = "libcgj-batch7996";

/* Product libcgj green lamp for wave 8000. */
#define B7996_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7996_green(void)
{
	return B7996_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_8000 - report product libcgj green lamp for wave 8000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_8000(void)
{
	(void)NULL;
	return b7996_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_8000(void)
    __attribute__((alias("gj_libcgj_green_8000")));
