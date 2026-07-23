/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5996: libcgj product green extract (wave 6000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_6000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       6000 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_6000  (alias)
 *   __libcgj_batch5996_marker = "libcgj-batch5996"
 *
 * Milestone 6000 exclusive continuum CREATE-ONLY (5991-6000). Unique
 * gj_libcgj_green_6000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_5900 (batch5896), gj_libcgj_green_5800 (batch5796),
 * gj_libcgj_green_5700 (batch5696), and gj_shell_green_6000
 * (batch5995). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5996_marker[] = "libcgj-batch5996";

/* Product libcgj green lamp for wave 6000. */
#define B5996_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5996_green(void)
{
	return B5996_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_6000 - report product libcgj green lamp for wave 6000.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_6000(void)
{
	(void)NULL;
	return b5996_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_6000(void)
    __attribute__((alias("gj_libcgj_green_6000")));
