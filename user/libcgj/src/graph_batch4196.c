/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4196: libcgj product green extract (wave 4200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4200(void);
 *     - Returns 1: milestone 4200 continuum product libcgj is green.
 *       Soft compile-time product tag; not a runtime dlopen or soname
 *       probe.
 *   uint32_t __gj_libcgj_green_4200  (alias)
 *   __libcgj_batch4196_marker = "libcgj-batch4196"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_libcgj_green_4200 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4100 (batch4096), gj_libcgj_green_3950 (batch3946),
 * gj_libcgj_green_3900 (batch3896), and gj_shell_green_4200
 * (batch4195). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4196_marker[] = "libcgj-batch4196";

/* Product libcgj green lamp for wave 4200. */
#define B4196_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4196_green(void)
{
	return B4196_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4200 - report milestone-4200 product libcgj green status.
 *
 * Always returns 1 (libcgj green). Does not parse soname strings and
 * does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4200(void)
{
	(void)NULL;
	return b4196_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4200(void)
    __attribute__((alias("gj_libcgj_green_4200")));
