/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4546: libcgj product green extract (wave 4550).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4550(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4550 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4550  (alias)
 *   __libcgj_batch4546_marker = "libcgj-batch4546"
 *
 * Milestone 4550 exclusive continuum CREATE-ONLY (4541-4550). Unique
 * gj_libcgj_green_4550 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4500 (batch4496), gj_libcgj_green_4400 (batch4396),
 * gj_libcgj_green_4300 (batch4296), and gj_shell_green_4550
 * (batch4545). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4546_marker[] = "libcgj-batch4546";

/* Product libcgj green lamp for wave 4550. */
#define B4546_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4546_green(void)
{
	return B4546_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4550 - report product libcgj green lamp for wave 4550.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4550(void)
{
	(void)NULL;
	return b4546_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4550(void)
    __attribute__((alias("gj_libcgj_green_4550")));
