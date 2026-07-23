/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4146: libcgj product green status (wave 4150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_4150(void);
 *     - Returns the product libcgj green/ready status flag for the
 *       milestone 4150 continuum (always 1 = green). Soft compile-time
 *       product status lamp; not a runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_4150  (alias)
 *   __libcgj_batch4146_marker = "libcgj-batch4146"
 *
 * Milestone 4150 exclusive continuum CREATE-ONLY (4141-4150). Unique
 * gj_libcgj_green_4150 surface only; no multi-def. Distinct from
 * gj_libcgj_green_4100 (batch4096), gj_libcgj_green_4050 (batch4046),
 * and gj_shell_green_4150 (batch4145). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4146_marker[] = "libcgj-batch4146";

/* Product libcgj green status for wave 4150 (1 = green). */
#define B4146_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4146_green(void)
{
	return B4146_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_4150 - report product libcgj green status for wave 4150.
 *
 * Always returns 1 (libcgj green for milestone 4150 continuum). Does
 * not parse soname strings and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_4150(void)
{
	(void)NULL;
	return b4146_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_4150(void)
    __attribute__((alias("gj_libcgj_green_4150")));
