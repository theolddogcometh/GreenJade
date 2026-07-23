/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4296: libcgj product green extract (wave 4300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       4300 continuum). Soft compile-time product status tag; not a
 *       runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4300  (alias)
 *   __libcgj_batch4296_marker = "libcgj-batch4296"
 *
 * Milestone 4300 exclusive continuum CREATE-ONLY (4291-4300). Unique
 * gj_libcgj_green_4300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4100 (batch4096), gj_libcgj_green_3950 (batch3946),
 * gj_libcgj_green_3900 (batch3896), and gj_shell_green_4300
 * (batch4295). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4296_marker[] = "libcgj-batch4296";

/* Product libcgj green lamp for wave 4300. */
#define B4296_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4296_green(void)
{
	return B4296_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4300 - report product libcgj green lamp for wave 4300.
 *
 * Always returns 1 (libcgj green/ready). Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4300(void)
{
	(void)NULL;
	return b4296_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4300(void)
    __attribute__((alias("gj_libcgj_green_4300")));
