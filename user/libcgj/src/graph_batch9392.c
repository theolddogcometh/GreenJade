/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9392: libcgj product green extract (wave 9400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_9400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       9400 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_9400  (alias)
 *   __libcgj_batch9392_marker = "libcgj-batch9392"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_libcgj_green_9400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_9300 (batch9292), gj_libcgj_green_9200 (batch9192),
 * gj_libcgj_green_9100 (batch9092), and gj_shell_green_9400
 * (batch9391). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9392_marker[] = "libcgj-batch9392";

/* Product libcgj green lamp for wave 9400. */
#define B9392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9392_green(void)
{
	return B9392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_9400 - report product libcgj green lamp for wave 9400.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_9400(void)
{
	(void)NULL;
	return b9392_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_9400(void)
    __attribute__((alias("gj_libcgj_green_9400")));
